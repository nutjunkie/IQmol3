/*******************************************************************************

  Copyright (C) 2011-2015 Andrew Gilbert

  This file is part of IQmol, a free molecular visualization program. See
  <http://iqmol.org> for more details.

  IQmol is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  IQmol is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along
  with IQmol.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************************/

#include "ViewerModel.h"

#include "Layer/FrequenciesLayer.h"
#include "Layer/GeometryListLayer.h"
#include "Layer/GeometryLayer.h"
#include "Layer/SurfaceLayer.h"
#include "Data/MacroMolecule.h"
#include "Preferences.h"
#include "UndoCommands.h"
#include "QMsgBox.h"
#include "Exception.h"
#include "QVariantPtr.h"
#include "CartesianCoordinatesParser.h"
#include "TextStream.h"
#include "ParseJobFiles.h"
#include "OpenBabelParser.h"
#include "YamlNode.h"
#include "SymmetryToleranceDialog.h"
#include "ServerConfiguration.h"
#include "ServerConfigurationDialog.h"
#include <QStringList>
#include <QStandardItem>
#include "QsLog.h"
#include <QUrl>
#include <QDir>
#include <QClipboard>
#include <QMimeData>
#include <cmath>
#include <algorithm>

#include <QtDebug>


using namespace qglviewer;


namespace IQmol {

ViewerModel::ViewerModel(QWidget* parent) : 
   QStandardItemModel(0, 1, parent),
   m_parent(parent), 
   m_global("Global", parent),
   m_symmetryTolerance(Preferences::SymmetryTolerance()), 
   m_forceField(Preferences::DefaultForceField()), 
   m_updateEnabled(true)
{
   QStringList labels;
   labels << "Model View";
   setHorizontalHeaderLabels(labels);
   invisibleRootItem()->setCheckState(Qt::Checked);
   m_global.setFlags(Qt::ItemIsEnabled);
   appendRow(&m_global);

   m_global.appendRow(&m_background);
   m_global.appendRow(&m_axes);
   m_global.appendRow(&m_mesh);
   m_global.appendRow(&m_clippingPlane);

   connect(&m_background, SIGNAL(updated()), 
      this, SIGNAL(updated()));
   connect(&m_background, SIGNAL(foregroundColorChanged(QColor const&)), 
      this, SIGNAL(foregroundColorChanged(QColor const&))); 
   connect(&m_background, SIGNAL(backgroundColorChanged(QColor const&)), 
      this, SIGNAL(backgroundColorChanged(QColor const&))); 

   connect(&m_axes, SIGNAL(updated()), 
      this, SIGNAL(updated()));
   connect(&m_mesh, SIGNAL(updated()), 
      this, SIGNAL(updated()));
   connect(&m_clippingPlane, SIGNAL(updated()), 
      this, SIGNAL(updated()));

   connect(this, SIGNAL(sceneRadiusChanged(double const)), 
      &m_axes, SLOT(setSceneRadius(double const)));

   connect(this, SIGNAL(sceneRadiusChanged(double const)), 
      &m_mesh, SLOT(setSceneRadius(double const)));

   connect(this, SIGNAL(sceneRadiusChanged(double const)), 
      &m_clippingPlane, SLOT(setSceneRadius(double const)));

   connect(this, SIGNAL(itemChanged(QStandardItem*)), 
      this, SLOT(checkItemChanged(QStandardItem*)));

   // By default we create a new Molecule, Systems are more 
   // conveniently loaded from file(s)
   Layer::Molecule* mol(newMolecule());
   appendRow(mol);

   changeActiveViewerMode(Viewer::BuildAtom);
   sceneRadiusChanged(Preferences::DefaultSceneRadius());
}


QStringList ViewerModel::mimeTypes () const
{
   QStringList types;
   types << "text/uri-list";
   return types;
}


Qt::DropActions ViewerModel::supportedDropActions () const
{
    return Qt::CopyAction | Qt::MoveAction;
}


bool ViewerModel::dropMimeData(QMimeData const* data, Qt::DropAction, int, int,
   const QModelIndex& index) 
{
   qDebug() << "Viewer Model drop activated";
   QStandardItem* item = itemFromIndex(index);

   if (item && data) {
      qDebug() << "Item dumped on:" << item->text();
      if (data->hasUrls()) {
         QList<QUrl> urls(data->urls());
         QList<QUrl>::iterator iter;

         for (iter = urls.begin(); iter != urls.end(); ++iter) {
            qDebug() << "from dropMimeData:" << (*iter);
         }
      }
   }
   
   return true;
}



// - - - - - Global Guff  - - - - -

void ViewerModel::displayGlobals()
{
   m_background.draw();
   m_axes.draw();
   m_mesh.draw();
}


// This is needed for the Axes but not the Mesh as the axes 
// can be controlled via the menu
void ViewerModel::toggleAxes()
{
   if (m_axes.checkState() == Qt::Checked) {
      m_axes.setCheckState(Qt::Unchecked);
      axesOn(false);
   }else {
      m_axes.setCheckState(Qt::Checked);
      axesOn(true);
   }
}


void ViewerModel::adjustSymmetryTolerance() 
{
   SymmetryToleranceDialog dialog(m_parent, m_symmetryTolerance);
   connect(&dialog, SIGNAL(symmetrizeRequest(double const)),
      this, SLOT(symmetrize(double const)));
   dialog.exec();

   if (dialog.result() == QDialog::Accepted) m_symmetryTolerance = dialog.value();
}


double ViewerModel::sceneRadius(bool visibleOnly)
{
   double radius(Preferences::DefaultSceneRadius());

   unsigned int findFlags(Layer::Children);
   if (visibleOnly) findFlags = (findFlags | Layer::Visible);

   ComponentList components = findLayers<Layer::Component>(findFlags);
   for (auto iter = components.begin(); iter != components.end(); ++iter) {
       radius = std::max(radius, (*iter)->radius());
   }

   return radius;
}


void ViewerModel::insertMoleculeById(QString identifier)
{
   Parser::OpenBabel parser;
   QString ext;

   qDebug() << "Identifier" << identifier;

   if (identifier.indexOf("SMILES:") == 0) {
      identifier.replace(0,7,"");
      ext = "smi";

   }else if (identifier.indexOf("INCHI:") == 0) {
      identifier.replace(0,6,"");
      if (!identifier.startsWith("InChI=")) identifier.prepend("InChI=");
      ext = "inchi";

   }else {
      QLOG_WARN() << "Unknown molecule identifier type:" << identifier;
      return;
   }

   bool ok(parser.parse(identifier, ext));
   QStringList errors(parser.errors());
   Data::Bank& bank(parser.data());

   if (ok && errors.isEmpty() && !bank.isEmpty()) {
      if (activeMolecule()) activeMolecule()->appendData(bank);
   }else {
      QMsgBox::warning(m_parent, "IQmol", "Invalid string");
   }
}


void ViewerModel::mergeSelection(QModelIndexList const& selection)
{
   Layer::Molecule* molecule;
   Layer::Molecule* parent(0);

   MoleculeList molecules;

   AtomList atoms;

   for (auto item : selection) {
       Layer::Base* base = QVariantPtr<Layer::Base>::toPointer(item.data(Qt::UserRole+1));
       if ( (molecule = qobject_cast<Layer::Molecule*>(base)) ) {
          if (parent == 0) {
             parent = molecule;
          } else {
             atoms.append(molecule->findLayers<Layer::Atom>());
             molecules.append(molecule);
          }
       }
   }

   Layer::PrimitiveList primitives;
   for (auto atom : atoms) {
       auto a(new Layer::Atom(atom->getAtomicNumber(), atom->getLabel()));
       a->setPosition(atom->getPosition());
       primitives.append(a);
   }

   for (auto molecule : molecules) removeMolecule(molecule);

   if (!parent) {
      QLOG_WARN() << "No parent molecule found for mergeSelection";
   }else {
      parent->appendPrimitives(primitives);
      parent->reperceiveBonds(false);
   }
}


void ViewerModel::hideMolecules(QModelIndexList const& selection)
{
   Layer::Molecule* molecule;

   for (auto item : selection) {
       Layer::Base* base = QVariantPtr<Layer::Base>::toPointer(item.data(Qt::UserRole+1));
       if ( (molecule = qobject_cast<Layer::Molecule*>(base)) ) {
          molecule->setCheckState(Qt::Unchecked);
       }
   }
}


void ViewerModel::showMolecules(QModelIndexList const& selection)
{
   Layer::Molecule* molecule;

   for (auto item : selection) {
       Layer::Base* base = QVariantPtr<Layer::Base>::toPointer(item.data(Qt::UserRole+1));
       if ( (molecule = qobject_cast<Layer::Molecule*>(base)) ) {
          molecule->setCheckState(Qt::Checked);
       }
   }
}

void ViewerModel::deleteSelection(QModelIndexList const&)
{
   qDebug() << "ViewerModel::deleteSelection NYI !!!";
}


void ViewerModel::newMoleculeFromSelection(QModelIndexList const& selection)
{
   Layer::Atom*     atom;
   Layer::Group*    group;
   Layer::Molecule* molecule;

   AtomList atoms;

   for (auto item : selection) {
       Layer::Base* base = QVariantPtr<Layer::Base>::toPointer(item.data(Qt::UserRole+1));

       if ( (group = qobject_cast<Layer::Group*>(base)) ) {
          atoms.append(group->getAtoms());
       }else if ( (atom = qobject_cast<Layer::Atom*>(base)) ) {
           atoms.append(atom);
       }else if ( (molecule = qobject_cast<Layer::Molecule*>(base)) ) {
           atoms.append(molecule->findLayers<Layer::Atom>());
       }
   }

   newMoleculeRequested(atoms);
}


void ViewerModel::newMoleculeRequested(AtomList const& atomList)
{
   if (atomList.isEmpty()) return;

   // Attempt to find a system to attach the new molecule, otherwise it will 
   // attach to the root item.
   SystemList parents(
      atomList.first()->findLayers<Layer::System>(Layer::Parents | Layer::Nested));

   QStandardItem* root = parents.isEmpty() ? invisibleRootItem() : parents.first();

   Layer::PrimitiveList primitives;
   for (auto atom : atomList) {
       auto a(new Layer::Atom(atom->getAtomicNumber(), atom->getLabel()));
       a->setPosition(atom->getPosition());
       primitives.append(a);
   }

   Layer::Molecule* molecule = newMolecule();
   molecule->appendPrimitives(primitives);
   molecule->reperceiveBonds(false);

   Command::AddMolecule* cmd(new Command::AddMolecule(molecule, root));
   postCommand(cmd);
}


// - - - - - Component - - - - -

void ViewerModel::connectComponent(Layer::Component* component) 
{
   connect(component, SIGNAL(updated()), 
      this, SLOT(updateObjectLists()));

   connect(component, SIGNAL(softUpdate()), 
      this, SIGNAL(updated()));

   connect(component, SIGNAL(postMessage(QString const&)), 
      this, SIGNAL(displayMessage(QString const&)));

   connect(component, SIGNAL(postCommand(QUndoCommand*)), 
      this, SIGNAL(postCommand(QUndoCommand*)));

   connect(component, SIGNAL(pushAnimators(AnimatorList const&)), 
      this, SIGNAL(pushAnimators(AnimatorList const&)));

   connect(component, SIGNAL(popAnimators(AnimatorList const&)), 
      this, SIGNAL(popAnimators(AnimatorList const&)));

   connect(component, SIGNAL(select(QModelIndex const&, QItemSelectionModel::SelectionFlags)), 
      this, SIGNAL(select(QModelIndex const&, QItemSelectionModel::SelectionFlags)));
}


// - - - - - Molecule - - - - -

void ViewerModel::newMoleculeMenu()
{
   forAllSystems(
      std::bind(&Layer::System::setCheckState, std::placeholders::_1, Qt::Unchecked)
   );

   forAllMolecules(
      std::bind(&Layer::Molecule::setCheckState, std::placeholders::_1, Qt::Unchecked)
   );

   QStandardItem* parent(invisibleRootItem());
   Command::AddMolecule* cmd(new Command::AddMolecule(newMolecule(), parent));
   changeActiveViewerMode(Viewer::BuildAtom);
   postCommand(cmd);
}


Layer::Molecule* ViewerModel::newMolecule()
{
   Layer::Molecule* molecule = new Layer::Molecule(m_parent);
   connectComponent(molecule);

   connect(molecule, SIGNAL(updated()), 
      this, SLOT(computeEnergy()));

   connect(molecule, SIGNAL(removeMolecule(Layer::Molecule*)), 
      this, SLOT(removeMolecule(Layer::Molecule*)));

   return molecule;
}


void ViewerModel::removeMolecule(Layer::Molecule* molecule)
{
qDebug() << "ViewerModel::removeMolecule called" << molecule;
   QList<Layer::Base*> parents(molecule->findLayers<Layer::Base>(Layer::Parents));
   
   if (parents.isEmpty()) {
      postCommand(new Command::RemoveMolecule(molecule, invisibleRootItem()));
   }else {
      postCommand(new Command::RemoveMolecule(molecule, parents.first()));
   }
}


// This should probably be made smarter, for the time being we simply return
// the last visible molecule.
Layer::Molecule* ViewerModel::activeMolecule()
{
   MoleculeList molecules = moleculeList();
   return molecules.isEmpty() ? 0 : molecules.last();
}


MoleculeList ViewerModel::moleculeList(bool visibleOnly)
{
   unsigned int findFlags(Layer::Children);
   if (visibleOnly) findFlags = (findFlags | Layer::Visible);
   return findLayers<Layer::Molecule>(findFlags);
}


void ViewerModel::forAllMolecules(std::function<void(Layer::Molecule&)> function)
{
   bool visibleOnly(true);
   MoleculeList molecules(moleculeList(visibleOnly));
   MoleculeList::iterator iter;
   for (iter = molecules.begin(); iter != molecules.end(); ++iter) {
       function(*(*iter));
   }
}


// - - - - - System - - - - - 
Layer::System* ViewerModel::newSystem()
{
   Layer::System* system = new Layer::System(DefaultMoleculeName, m_parent);
   connectComponent(system);

   connect(system, SIGNAL(removeSystem(Layer::System*)), 
      this, SLOT(removeSystem(Layer::System*)));

   connect(system, SIGNAL(removeMolecule(Layer::Molecule*)), 
      this, SLOT(removeMolecule(Layer::Molecule*)));

   connect(system, SIGNAL(connectComponent(Layer::Component*)),
      this, SLOT(connectComponent(Layer::Component*)));

   connect(system, SIGNAL(newMoleculeRequested(AtomList const&)), 
      this, SLOT(newMoleculeRequested(AtomList const&)));

   // Used to update the Octree selection volume
   connect(this, SIGNAL(selectionChanged()), 
      system, SIGNAL(selectionChanged()));

   return system;
}


void ViewerModel::removeSystem(Layer::System* system)
{
   postCommand(new Command::RemoveSystem(system, invisibleRootItem()));
}


// This should probably be made smarter, for the time being we simply return
// the last visible system.
Layer::System* ViewerModel::activeSystem()
{
   SystemList systems = systemList();
   return  systems.isEmpty() ? 0 : systems.last();
}


SystemList ViewerModel::systemList(bool visibleOnly)
{
   unsigned int findFlags(Layer::Children);
   if (visibleOnly) findFlags = (findFlags | Layer::Visible);
   return findLayers<Layer::System>(findFlags);
}


void ViewerModel::forAllSystems(std::function<void(Layer::System&)> function)
{
   bool visibleOnly(true);
   SystemList systems(systemList(visibleOnly));
   SystemList::iterator iter;
   for (iter = systems.begin(); iter != systems.end(); ++iter) {
       function(*(*iter));
   }
}


Layer::Component* ViewerModel::activeComponent()
{
   Layer::Component* comp(activeSystem());
   return (comp ? comp : activeMolecule());
}



// - - - - - I/O - - - - -

void ViewerModel::open(QString const& filePath, QString const& filter, void* moleculePointer)
{
   QString path(filePath);
   while (path.endsWith("/")) {
       path.chop(1);
   }
#ifdef Q_OS_WIN32
   // Not sure where this is getting added, but it causes problems on Windows, obviously
   while (path.startsWith("/")) {
       path.remove(0,1);
   }
#endif
   ParseJobFiles* parser = new ParseJobFiles(path, filter, moleculePointer);
   connect(parser, SIGNAL(finished()), this, SLOT(fileOpenFinished()));
   parser->start();
}


void ViewerModel::fileOpenFinished()
{
   ParseJobFiles* parser = qobject_cast<ParseJobFiles*>(sender());
   if (!parser) return;
   if (parser->status() == Task::SigTrap) throw SignalException();

   Data::Bank& bank(parser->data());
   QFileInfo info(parser->filePath());
   QStringList errors(parser->errors());

   if (bank.isEmpty()) {
      if (errors.isEmpty()) errors.append("No valid data found in file " + info.filePath());
      QMsgBox::warning(m_parent, "IQmol", errors.join("\n"));
      parser->deleteLater();
      return;
   }

   if (!errors.isEmpty()) {
      QMsgBox::warning(m_parent, "IQmol", errors.join("\n"));
   }

   processConfigData(bank);
   processParsedData(parser);
   parser->deleteLater();
}


void ViewerModel::saveAll()
{
   forAllMolecules(std::bind(&Layer::Molecule::save, std::placeholders::_1, false));
}


void ViewerModel::saveAs()
{
   Layer::Molecule* molecule(activeMolecule());
   if (molecule) molecule->save(true);
}


bool ViewerModel::saveAllAndClose()
{
   bool visibleOnly(false);
   MoleculeList molecules(moleculeList(visibleOnly));
   MoleculeList::iterator iter;
   for (iter = molecules.begin(); iter != molecules.end(); ++iter) {
       qDebug() << "saveAllandClose" << (*iter)->text();
       if ((*iter)->save() == false) return false;
   }
   return true;
}


bool ViewerModel::saveRequired()
{
   bool save(false);
   bool visibleOnly(false);
   MoleculeList molecules(moleculeList(visibleOnly));
   MoleculeList::iterator iter;
   for (iter = molecules.begin(); iter != molecules.end(); ++iter) {
       save = save || (*iter)->isModified();
   }
   return save;
}



void ViewerModel::processConfigData(Data::Bank& bank)
{
   QList<Data::YamlNode*> yaml(bank.takeData<Data::YamlNode>());
   for (int i = 0; i < yaml.size(); ++i) {
       yaml[i]->dump();
       Process::ServerConfiguration config(*(yaml[i]));
       Process::ServerConfigurationDialog dialog(config); 
       dialog.exec();
   }
}


void ViewerModel::processParsedData(ParseJobFiles* parser)
{
   Data::Bank& bank(parser->data());

   auto mm(bank.findData<Data::MacroMolecule>());
   if (mm.isEmpty()) {
      processMoleculeData(parser);
   }else {
      processSystemData(parser);
   }
}


void ViewerModel::processSystemData(ParseJobFiles* parser)
{
   QString name(parser->name());
   Data::Bank& bank(parser->data());
   if (bank.isEmpty()) return;

   // Determine if we need to replace an existing System in the Model View.
   bool overwrite(parser->flags()  & ParseJobFiles::Overwrite);
   bool makeActive(parser->flags() & ParseJobFiles::MakeActive);
   bool addStar(parser->flags()    & ParseJobFiles::AddStar);
   bool found(false);

   Layer::System* system(newSystem());
   system->setFile(parser->filePath()); 
   system->setCheckState(Qt::Unchecked);
   system->setText(name);
   system->appendData(bank);
   if (addStar) system->setIcon(QIcon(":/icons/Favourites.png"));

   QStandardItem* child;
   QStandardItem* root(invisibleRootItem());

   void* systemPointer(parser->moleculePointer());

   if (overwrite && systemPointer) {
      for (int row = 0; row < root->rowCount(); ++row) {
          child = root->child(row);
          if (child->text() == name) {
             Layer::Base*  base = QVariantPtr<Layer::Base>::toPointer(child->data());
             Layer::System* sys = qobject_cast<Layer::System*>(base);

             if (system && system == systemPointer) {
                QLOG_TRACE() << "Found existing system";
                system->setCheckState(sys->checkState());
                // makeActive = makeActive || (mol->checkState() == Qt::Checked);
                // This may result in a memory leak, but we need the System
                // to remain lying around in case of an undo action.
                sys->disconnect();
                takeRow(row);
                insertRow(row, system);
                found = true;
                break;
             }
          }
      }
   }

   // If we didn't find an existing System to overwrite, we check the last
   // System on the list and if that is still 'Untitled' and empty, use it.
   if (!found) {
      child = root->child(root->rowCount()-1);
      if (child->text() == DefaultMoleculeName) {
         Layer::Base* base = QVariantPtr<Layer::Base>::toPointer(child->data());
         Layer::System* sys = qobject_cast<Layer::System*>(base);
         makeActive = makeActive || (sys->checkState() == Qt::Checked);
         // This may result in a memory leak, but we need the Molecule
         // to remain lying around in case of an undo action.
         sys->disconnect();
         takeRow(child->row());
      }

      Command::AddSystem* cmd = new Command::AddSystem(system, root);
      postCommand(cmd);
   }

   if (makeActive) {
      forAllSystems(
         std::bind(&Layer::System::setCheckState, std::placeholders::_1, Qt::Unchecked)
      );
      system->setCheckState(Qt::Checked);
      sceneRadiusChanged(sceneRadius());
      changeActiveViewerMode(Viewer::Manipulate);
   }

   fileOpened(parser->filePath());
}


void ViewerModel::processMoleculeData(ParseJobFiles* parser)
{
   Data::Bank& bank(parser->data());
   QString name(parser->name());

   // Determine if we need to replace an existing Molecule in the Model View.
   bool overwrite(parser->flags()  & ParseJobFiles::Overwrite);
   bool makeActive(parser->flags() & ParseJobFiles::MakeActive);
   bool addStar(parser->flags()    & ParseJobFiles::AddStar);
   bool found(false);

/*
   qDebug() << "==============================================================";
   qDebug() << "Dumping bank contents";
   qDebug() << "==============================================================";
   bank.dump();
*/
   Layer::Molecule* molecule(newMolecule());
   molecule->setCheckState(Qt::Unchecked);
   molecule->setText(name);
   molecule->appendData(bank);
   if (addStar) molecule->setIcon(QIcon(":/icons/Favourites.png"));

   QStandardItem* child;
   QStandardItem* root(invisibleRootItem());

   void* moleculePointer(parser->moleculePointer());

   if (overwrite && moleculePointer) {
      for (int row = 0; row < root->rowCount(); ++row) {
          child = root->child(row);
          if (child->text() == name) {
             Layer::Base* base = QVariantPtr<Layer::Base>::toPointer(child->data());
             Layer::Molecule* mol = qobject_cast<Layer::Molecule*>(base);

             if (mol && mol == moleculePointer) {
                QLOG_TRACE() << "Found existing molecule";
                molecule->setCheckState(mol->checkState());
                // makeActive = makeActive || (mol->checkState() == Qt::Checked);
                // This may result in a memory leak, but we need the Molecule
                // to remain lying around in case of an undo action.
                mol->disconnect();
                takeRow(row);
                insertRow(row, molecule);
                found = true;
                break;
             }
          }
      }
   }

   // If we didn't find an existing Molecule to overwrite, we check the last
   // Molecule on the list and if that is still 'Untitled' and empty, use it.
   if (!found) {
      child = root->child(root->rowCount()-1);
      if (child->text() == DefaultMoleculeName && !child->hasChildren()) {
         Layer::Base* base = QVariantPtr<Layer::Base>::toPointer(child->data());
         Layer::Molecule* mol = qobject_cast<Layer::Molecule*>(base);
         makeActive = makeActive || (mol->checkState() == Qt::Checked);
         // This may result in a memory leak, but we need the Molecule
         // to remain lying around in case of an undo action.
         mol->disconnect();
         takeRow(child->row());
      }

      Command::AddMolecule* cmd = new Command::AddMolecule(molecule, root);
      postCommand(cmd);
   }

   if (makeActive) {
      forAllMolecules(
         std::bind(&Layer::Molecule::setCheckState, std::placeholders::_1, Qt::Unchecked)
      );
      forAllSystems(
         std::bind(&Layer::System::setCheckState, std::placeholders::_1, Qt::Unchecked)
      );

      molecule->setCheckState(Qt::Checked);
      sceneRadiusChanged(sceneRadius());
      changeActiveViewerMode(Viewer::Manipulate);
   }

   fileOpened(parser->filePath());
}



// ---------- Clipboard ----------

// Note that we can only paste a string containing a list of coordinates,
// nothing else.  There appears to be an OS X specific bug that prevents this
// from working properly.  The clipboard text gets mangled if the clipboard
// contents originates from another application.
// http://www.qtforum.org/article/28645/qclipboard-problem-mac-osx-tiger.html#post97862
void ViewerModel::pasteSelectionFromClipboard()
{
   QString xyz(QApplication::clipboard()->text(QClipboard::Clipboard));
   Parser::CartesianCoordinates parser;
   Parser::TextStream textStream(&xyz);
   Data::Geometry* geom(parser.parse(textStream));

   if (geom && activeMolecule()) {
      selectAll(); 
      // This should be added as an Undo action
      activeMolecule()->appendPrimitives(Layer::PrimitiveList(*geom));
      invertSelection(); 
      updateObjectLists();
      delete geom;
   }
}


void ViewerModel::copySelectionToClipboard()
{
   MoleculeList molecules(moleculeList());
   MoleculeList::iterator mol;
    
   bool selectedOnly(true);
   QString coordinates;
   for (mol = molecules.begin(); mol != molecules.end(); ++mol) {
       coordinates += (*mol)->coordinatesAsString(selectedOnly);
   }

   qDebug() << coordinates;
   QApplication::clipboard()->setText(coordinates);
}


void ViewerModel::cutSelection()
{
   copySelectionToClipboard();
   deleteSelection();
}


void ViewerModel::groupSelection()
{
   forAllMolecules(
      std::bind(&Layer::Molecule::groupSelection, std::placeholders::_1)
   );
}


void ViewerModel::ungroupSelection()
{
   forAllMolecules(
      std::bind(&Layer::Molecule::ungroupSelection, std::placeholders::_1)
   );
}


void ViewerModel::updateObjectLists()
{
   if (!m_updateEnabled) return;

   // We don't want nested objects as Fragments should appear as one object in
   // the Viewer.  This means the Fragment is respnsible for drawing its children
   GLObjectList allObjects = findLayers<Layer::GLObject>(Layer::Children | Layer::Visible | 
      Layer::Nested);

   // Make sure the selection only contains visible objects;
   GLObjectList::iterator object(m_selectedObjects.begin());
   while (object != m_selectedObjects.end()) {
       if ( allObjects.contains(*object)) {
          ++object;
       } else {
          (*object)->deselect();
          object = m_selectedObjects.erase(object);
       }
   }

   m_opaqueObjects.clear();
   m_transparentObjects.clear();

   for (auto object : allObjects) {
       if (object->isTransparent()) {
          m_transparentObjects << object;
       }else {
          m_opaqueObjects << object;
       }
   }

   // Sort transparent objects based on opacity, high to low
   std::sort(m_transparentObjects.begin(), 
             m_transparentObjects.end(), 
             Layer::GLObject::AlphaSort);
   // Hack to ensure that atoms (which are given alpha = 0.999 are drawn after bonds
   std::sort(m_opaqueObjects.begin(), 
             m_opaqueObjects.end(), 
             Layer::GLObject::AlphaSort);
   updated();
}



// --------------- Selection Routines ---------------

// This is the main selection routine which should only receive selection
// signals from m_viewerSelectionModel.
void ViewerModel::selectionChanged(QItemSelection const& selected, 
   QItemSelection const& deselected)
{
   QModelIndexList list;
   QModelIndexList::iterator iter; 
   Layer::Base* base;
   Layer::Mode* mode;
   Layer::Geometry* geometry;
   Layer::GeometryList* geometryList1(0);
   Layer::GeometryList* geometryList2(0);
   Layer::GLObject* glObject;
   Layer::Molecule* molecule;
   bool setDefaultGeometry(false);

   list = deselected.indexes();
//qDebug() << "ViewerModel::selectionChanged  deselected" << list;
   for (iter = list.begin(); iter != list.end(); ++iter) {
       base = QVariantPtr<Layer::Base>::toPointer((*iter).data(Qt::UserRole+1));
       if ( (glObject = qobject_cast<Layer::GLObject*>(base)) ) {
          glObject->deselect();
          m_selectedObjects.removeAll(glObject);

       }else if ( (molecule = qobject_cast<Layer::Molecule*>(base)) ) {
          if (molecule->checkState() == Qt::Checked) {
          GLObjectList objects(molecule->findLayers<Layer::GLObject>());
          for (auto object : objects) {
              object->deselect();
              m_selectedObjects.removeAll(object);
          }
          }

       }else if ( (mode = qobject_cast<Layer::Mode*>(base)) ) {
          QStandardItem* parent(mode->QStandardItem::parent());
          Layer::Frequencies* frequencies;
          base = QVariantPtr<Layer::Base>::toPointer(parent->data());
          if ( (frequencies = qobject_cast<Layer::Frequencies*>(base)) ) {
             frequencies->clearActiveMode();
          }

       }else if ( (geometry = qobject_cast<Layer::Geometry*>(base)) ) {
          QStandardItem* parent(geometry->QStandardItem::parent());
          base = QVariantPtr<Layer::Base>::toPointer(parent->data());
          // grab a handle on the geometry list here, but we only need to
          // reset the geometry if we don't select another geometry in the
          // next section
          if ( (geometryList1 = qobject_cast<Layer::GeometryList*>(base)) ) {
             setDefaultGeometry = true;
          }
       }
   }

   list = selected.indexes();
   for (iter = list.begin(); iter != list.end(); ++iter) {
       base = QVariantPtr<Layer::Base>::toPointer((*iter).data(Qt::UserRole+1));
       if ( (glObject = qobject_cast<Layer::GLObject*>(base)) ) {

              glObject->select();
              m_selectedObjects.append(glObject);

       }else if ( (molecule = qobject_cast<Layer::Molecule*>(base)) ) {
          if (molecule->checkState() == Qt::Checked) {
          GLObjectList objects(molecule->findLayers<Layer::GLObject>());
          for (auto object : objects) {
              object->select();
              m_selectedObjects.append(object);
          }
          }

       }else if ( (mode = qobject_cast<Layer::Mode*>(base)) ) {
          QStandardItem* parent(mode->QStandardItem::parent());
          Layer::Frequencies* frequencies;
          base = QVariantPtr<Layer::Base>::toPointer(parent->data());
          if ( (frequencies = qobject_cast<Layer::Frequencies*>(base)) ) {
             frequencies->setActiveMode(*mode);
             Layer::Atom::setDisplayVibrationVector(true);
          }

       }else if ( (geometry = qobject_cast<Layer::Geometry*>(base)) ) {
          QStandardItem* parent(geometry->QStandardItem::parent());
          base = QVariantPtr<Layer::Base>::toPointer(parent->data());
          if ( (geometryList2 = qobject_cast<Layer::GeometryList*>(base)) ) {
             geometryList2->setCurrentGeometry(geometry->row());
             // ------- also select the corresponding MOs --------
             // ---------------------------------------------------
          }
          if (geometryList2 == geometryList1) setDefaultGeometry = false;
       }
   }

   selectionChanged();
   if (setDefaultGeometry) geometryList1->resetGeometry();
   if (m_updateEnabled) updated();
}


// The following selection functions interact directly with the selection model.
void ViewerModel::invertSelection()
{
   QItemSelection select;
   QItemSelection deselect;
   
   GLObjectList::iterator iter;
   for (iter = m_opaqueObjects.begin(); iter != m_opaqueObjects.end(); ++iter) {
       if ((*iter)->isSelected()) {
          deselect.select((*iter)->index(), (*iter)->index());
       }else {
          select.select((*iter)->index(), (*iter)->index());
       }
   }

   selectionChanged(deselect, QItemSelectionModel::Deselect);
   selectionChanged(select, QItemSelectionModel::Select);
}


void ViewerModel::selectAll()
{
   QItemSelection select;
   GLObjectList::iterator iter;
   for (iter = m_opaqueObjects.begin(); iter != m_opaqueObjects.end(); ++iter) {
       if ( ! (*iter)->isSelected()) {
          select.append(QItemSelectionRange((*iter)->index()));
       }
   }

   selectionChanged(select, QItemSelectionModel::Select);
}


void ViewerModel::selectNone()
{
   enableUpdate(false);
   updateObjectLists();
   enableUpdate(true);

   QItemSelection all;
   GLObjectList::iterator iter;
   for (iter = m_opaqueObjects.begin(); iter != m_opaqueObjects.end(); ++iter) {
       all.append(QItemSelectionRange((*iter)->index()));
   }
   selectionChanged(all, QItemSelectionModel::Deselect);
}


void ViewerModel::deleteSelection()
{
   enableUpdate(false);
   forAllMolecules(
      std::bind(&Layer::Molecule::deleteSelection, std::placeholders::_1)
   );
   enableUpdate(true);
   updateObjectLists();
}


void ViewerModel::checkItemChanged(QStandardItem* item)
{
   if (item->isCheckable()) {
      // disconnect to avoid recursion 
      disconnect(this, SIGNAL(itemChanged(QStandardItem*)), 
         this, SLOT(checkItemChanged(QStandardItem*)));

      Layer::Base* base;

      if (item->checkState() == Qt::Checked) {
         // This removes the star icon that appears when there are new results
         item->setIcon(QIcon()); 
      }else {
         // close all the child configurators
         if ((base = dynamic_cast<Layer::Base*>(item))) {
            QList<Layer::Base*> children(base->findLayers<Layer::Base>(Layer::Children));
            QList<Layer::Base*>::iterator child;
            for (child = children.begin(); child != children.end(); ++child) {
                if (*child) (*child)->closeConfigurator();
            }
         }     
      }

      if ((base = dynamic_cast<Layer::Base*>(item))) {

         Layer::Surface* layer;
         if ((layer = dynamic_cast<Layer::Surface*>(base))) {
            layer->setCheckStatus(item->checkState());
         }

         Layer::Molecule* molecule;
         if ((molecule = dynamic_cast<Layer::Molecule*>(base))) {
            displayMessage("");
         }

         Layer::Background* background;
         if ((background = dynamic_cast<Layer::Background*>(base))) {
            displayMessage("");
         }
      }

      connect(this, SIGNAL(itemChanged(QStandardItem*)), 
         this, SLOT(checkItemChanged(QStandardItem*)));
      updateObjectLists();
   }
}


void ViewerModel::itemDoubleClicked(QModelIndex const& index)
{
   QStandardItem* item = itemFromIndex(index);
   Layer::Base* layer;
   if ((layer = dynamic_cast<Layer::Base*>(item))) layer->configure();
}


void ViewerModel::itemExpanded(QModelIndex const& index)
{
   // Remove the star if the molecule has new results
   QStandardItem* item = itemFromIndex(index);
   item->setIcon(QIcon()); 
}





// - - - - - Menu Slots - - - - -

void ViewerModel::setConstraint()
{
   Layer::Molecule* mol(activeMolecule());
   if (mol && mol->editConstraint()) selectNone();
}


void ViewerModel::freezeAtomPositions()
{
   Layer::Molecule* mol(activeMolecule());
   if (mol && mol->freezeAtomPositions()) selectNone();
}


void ViewerModel::setIsotopes()
{
   Layer::Molecule* mol(activeMolecule());
   if (mol && mol->editIsotopes()) selectNone();
}


void ViewerModel::addHydrogens()
{
   forAllMolecules(
      std::bind(&Layer::Molecule::addHydrogens, std::placeholders::_1)
   );
}


void ViewerModel::reperceiveBonds()
{
   forAllMolecules(
      std::bind(&Layer::Molecule::reperceiveBonds, std::placeholders::_1, true)
   );
}


void ViewerModel::reperceiveBondsForAnimation()
{
   forAllMolecules(
      std::bind(&Layer::Molecule::reperceiveBondsForAnimation, std::placeholders::_1)
   );
}


void ViewerModel::computeEnergy()
{
   forAllMolecules(
      std::bind(&Layer::Molecule::computeEnergy, std::placeholders::_1, m_forceField)
   );
}


void ViewerModel::minimizeEnergy()
{
   forAllMolecules(
      std::bind(&Layer::Molecule::minimizeEnergy, std::placeholders::_1, m_forceField)
   );
}


void ViewerModel::translateToCenter()
{
   // If we translate all Molecules and all Systems, the System Molecules 
   // get translated twice, so just do the active Component.

   Layer::System* system(activeSystem());
   if (system) {
      system->translateToCenter(m_selectedObjects);
   }else {
      Layer::Molecule* molecule(activeMolecule());
      molecule->translateToCenter(m_selectedObjects);
   }
}


void ViewerModel::symmetrize(double const tolerance)
{
   bool updateCoordinates(true);
   forAllMolecules(
      std::bind(&Layer::Molecule::symmetrize, std::placeholders::_1, tolerance, 
        updateCoordinates)
   );
}


void ViewerModel::toggleAutoDetectSymmetry()
{
   Layer::Molecule::toggleAutoDetectSymmetry();
}


void ViewerModel::saveToCurrentGeometry()
{
   forAllMolecules(
      std::bind(&Layer::Molecule::saveToCurrentGeometry, std::placeholders::_1)
   );
}


void ViewerModel::determineSymmetry()
{
   forAllMolecules(
      std::bind(&Layer::Molecule::autoDetectSymmetry, std::placeholders::_1)
   );
}

} // end namespace IQmol
