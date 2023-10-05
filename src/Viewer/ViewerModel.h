#pragma once
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

#include "Viewer.h"
#include "Layer/AxesMeshLayer.h"
#include "Layer/BackgroundLayer.h"
#include "Layer/ClippingPlaneLayer.h"
#include "Layer/AxesLayer.h"
#include "Layer/MoleculeLayer.h"
#include "Layer/SystemLayer.h"
#include <QStandardItemModel>
#include <QItemSelection>
#include <QList>
#include <QColor>

#include <functional>


class QStandardItem;
class QUndoCommand;

namespace IQmol {

   class ParseJobFiles;

   /// Model for the hierarchical data structures contained within the
   /// Viewer window.
   class ViewerModel : public QStandardItemModel {

      Q_OBJECT

      friend class Viewer;

      public:
         ViewerModel(QWidget* parent = 0);
         GLObjectList getVisibleObjects();
         GLObjectList getSelectedObjects();

         void displayGlobals();
         QColor backgroundColor() const { return m_background.color(); }

         void setForceField(QString const& forceField) { m_forceField = forceField; }
         double sceneRadius(bool visibleOnly = true);

         MoleculeList moleculeList(bool visibleOnly = true);
         Layer::Molecule* activeMolecule();

         SystemList systemList(bool visibleOnly = true);
         Layer::System* activeSystem();

         bool saveAllAndClose();
         bool saveRequired();

         Qt::DropActions supportedDropActions() const;
         QStringList mimeTypes () const;
         bool dropMimeData(QMimeData const* data, Qt::DropAction action, int row, 
            int column, const QModelIndex & parent);

      public Q_SLOTS:
		 /// Wrapper that creates a new Molecule and sets the Viewer in a 
         /// state ready for building.  The molecule is actually added via
         /// an AddMolecule Command.
         void newMoleculeMenu();
         void removeMolecule(Layer::Molecule*);
         void removeSystem(Layer::System*);

         void addHydrogens();
         void reperceiveBonds();
         void reperceiveBondsForAnimation();

         void symmetrize() { symmetrize(m_symmetryTolerance); }
         void symmetrize(double const);
         void determineSymmetry();
         void saveToCurrentGeometry();
         void adjustSymmetryTolerance();
         void toggleAutoDetectSymmetry();

         void freezeAtomPositions();
         void setConstraint();
         void setIsotopes();
         void translateToCenter();
         void minimizeEnergy();
         void computeEnergy();
         void insertMoleculeById(QString identifier);
         void newMoleculeFromSelection(QModelIndexList const&);
         void deleteSelection(QModelIndexList const&);
         void mergeSelection(QModelIndexList const&);

         void hideMolecules(QModelIndexList const&);
         void showMolecules(QModelIndexList const&);

         void cutSelection();
         void copySelectionToClipboard();
         void pasteSelectionFromClipboard();
         void deleteSelection();
         void invertSelection();
         void selectAll();
         void selectNone();
         void selectionChanged(QItemSelection const& selected,QItemSelection const& deselected);
         void groupSelection();
         void ungroupSelection();

         void enableUpdate(bool tf) { m_updateEnabled = tf; }
         void checkItemChanged(QStandardItem*);
         void itemDoubleClicked(QModelIndex const&);
         void itemExpanded(QModelIndex const&);
         void updateVisibleObjects();
         void toggleAxes();
         void saveAll();
         void saveAs();

         void fileOpenFinished();
         void open(QString const& fileName, QString const& filter = QString(),  
            qint64 moleculePointer = 0);


      Q_SIGNALS:
         void updated();
         void axesOn(bool);
         void sceneRadiusChanged(double const);
         void displayMessage(QString const&);
         void postCommand(QUndoCommand*);
         void selectionChanged(QItemSelection const& items, 
                 QItemSelectionModel::SelectionFlags);
         void select(QModelIndex const& item, QItemSelectionModel::SelectionFlags);
         void clearSelection();
         void changeActiveViewerMode(Viewer::Mode const);
         void pushAnimators(AnimatorList const&);
         void popAnimators(AnimatorList const&);
         void foregroundColorChanged(QColor const&);
         void backgroundColorChanged(QColor const&);
         void fileOpened(QString const&);

      protected:
         Layer::ClippingPlane& clippingPlane() { return  m_clippingPlane; }

      private:
		 /// Creates a new Molecule with the required connections to the
		 /// ViewerModel, but does not append the Molecule.  In most cases the
		 /// Molecule should be appended via an AddMolecule Command.
         Layer::Molecule* newMolecule();
         void forAllMolecules(std::function<void(Layer::Molecule&)> function);

         Layer::System* newSystem();
         void forAllSystems(std::function<void(Layer::System&)> function);

         // default is to find only the topmost visible Layers
         template <class T> 
         QList<T*> findLayers(unsigned int findFlags)
         {
            QStandardItem* root(invisibleRootItem());
            QStandardItem* child;
            Layer::Base* base;
            QList<T*> list;

            findFlags = findFlags | Layer::IncludeSelf;

            for (int i = 0; i < root->rowCount(); ++i) {
                child = root->child(i);
                base = QVariantPtr<Layer::Base>::toPointer(child->data());
                if (base) list += base->findLayers<T>(findFlags);
            }
            return list;
         }

         void processConfigData(Data::Bank&);
         void processParsedData(ParseJobFiles*);
         void processMoleculeData(ParseJobFiles*);
         void processSystemData(ParseJobFiles*);

         QWidget* m_parent;
         Layer::Base m_global;
         Layer::Axes m_axes;
         Layer::AxesMesh m_mesh;
         Layer::Background m_background;
         Layer::ClippingPlane m_clippingPlane;

         GLObjectList m_visibleObjects;
         GLObjectList m_selectedObjects;
         double m_symmetryTolerance;  // Hack, much.
         QString m_forceField;
         bool m_updateEnabled;
   };

} // end namespace IQmol
