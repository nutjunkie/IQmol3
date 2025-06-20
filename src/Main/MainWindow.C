/*******************************************************************************

  Copyright (C) 2022 Andrew Gilbert

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

#include "MainWindow.h"
#include "ServerConfigurationListDialog.h"
#include "JobMonitor.h"
#include "JobInfo.h"
#include "ServerRegistry.h" 
#include "InsertMoleculeDialog.h" 
#include "QMsgBox.h"
#include "FileDialog.h"
#include "Animator.h"
#include "Preferences.h"
#include "Network.h"
#include "Bank.h"
#include "Geometry.h"
#include "AtomicProperty.h"
#include "Atom.h"
#include "QsLog.h"

#ifdef GROMACS
#include "GromacsDialog.h" 
#endif

#include "Qui/InputDialog.h"
#include <QResizeEvent>
#include <QDropEvent>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QtDebug>
#include <QActionGroup>
#include <fstream>

#include "Util/ColorDialog.h"

namespace IQmol {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
   m_toolBar(this),
   m_aboutDialog(this),
   m_helpBrowser(this),
   m_viewerModel(this),
   m_viewerView(this),
   m_undoStack(this),
   m_undoStackView(&m_undoStack, this),
   m_statusWidget(this),
   m_viewerSelectionModel(&m_viewerModel, this),
   m_logMessageDialog(0),
   m_preferencesBrowser(this),
   m_quiInputDialog(0),
   m_gromacsDialog(0)
{
   m_viewer = new Viewer(m_viewerModel, this);

   setStatusBar(0);
   setWindowTitle("IQmol");
   setWindowModified(false);
   setAcceptDrops(false);
   setWindowIcon(QIcon(":/resources/icons/iqmol.png"));

   createMenus();
   createLayout();
   createConnections(); 

   m_undoStack.setUndoLimit(Preferences::UndoLimit());
   m_undoStackView.setEmptyLabel("History:");
   m_undoStackView.setSelectionMode(QAbstractItemView::SingleSelection);

   m_viewerView.setModel(&m_viewerModel);
   m_viewerView.setSelectionModel(&m_viewerSelectionModel);

   m_viewer->setActiveViewerMode(Viewer::BuildAtom);
   m_viewer->setDefaultSceneRadius();
   m_viewer->resetView();
   
   //Color::GetGradient(Color::Gradient::Default, this);
}


MainWindow::~MainWindow()
{
   if (m_quiInputDialog) delete m_quiInputDialog;
   delete m_viewer;
}


void MainWindow::setStatus()
{
}


void MainWindow::createLayout()
{
   QVBoxLayout* mainLayout = new QVBoxLayout;
   mainLayout->setContentsMargins(0,0,0,0);
   QWidget* center = new QWidget(this);
   center->setLayout(mainLayout); 
   setCentralWidget(center);
   resize(Preferences::MainWindowSize());

   mainLayout->addWidget(&m_toolBar);
   m_helpBrowser.setWindowFlags(Qt::Tool);
   m_statusWidget.showMessage("Welcome to IQmol");

   // sideSplitter (ha ha) is a data member as we need to control its visibility
   m_sideSplitter = new QSplitter(Qt::Vertical, this);
   m_sideSplitter->addWidget(&m_viewerView);
   m_sideSplitter->addWidget(&m_undoStackView);
   m_sideSplitter->addWidget(&m_statusWidget);
   m_sideSplitter->setCollapsible(0, true);
   m_sideSplitter->setCollapsible(1, true);

   m_statusWidget.show();

   QList<int> sizes;
   sizes << Preferences::MainWindowSize().height()- 220 - 12 << 220 << 12;
   m_sideSplitter->setSizes(sizes);

   // Main splitter
   QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
   splitter->addWidget(m_sideSplitter);
   splitter->addWidget(m_viewer);
   splitter->setCollapsible(0, true);
   splitter->setCollapsible(1, false);

   sizes.clear();
   if (Preferences::ShowModelView()) {
      sizes << 220 <<  Preferences::MainWindowSize().width()-200;
   }else {
      sizes << 0;
   }
   splitter->setSizes(sizes);

   mainLayout->addWidget(splitter);
   connect(splitter, SIGNAL(splitterMoved(int, int)), this, SLOT(splitterMoved(int, int)));
}


void MainWindow::createConnections()
{
   connect(&m_toolBar, SIGNAL(newMolecule()),     &m_viewerModel, SLOT(newMoleculeMenu()));
   connect(&m_toolBar, SIGNAL(open()),            this,           SLOT(openFile()));
   connect(&m_toolBar, SIGNAL(save()),            &m_viewerModel, SLOT(saveAll()));
   connect(&m_toolBar, SIGNAL(addHydrogens()),    &m_viewerModel, SLOT(addHydrogens()));
   connect(&m_toolBar, SIGNAL(minimizeEnergy()),  &m_viewerModel, SLOT(minimizeEnergy()));
   connect(&m_toolBar, SIGNAL(deleteSelection()), &m_viewerModel, SLOT(deleteSelection()));
   connect(&m_toolBar, SIGNAL(takeSnapshot()),    m_viewer,       SLOT(takeSnapshot()));
   connect(&m_toolBar, SIGNAL(record(bool)),      this,           SLOT(setRecord(bool)));
   connect(&m_toolBar, SIGNAL(fullScreen()),      this,           SLOT(fullScreen()));
   connect(&m_toolBar, SIGNAL(showHelp()),        this,           SLOT(showHelp()));

   connect(&m_toolBar, SIGNAL(viewerModeChanged(Viewer::Mode const)), 
      m_viewer, SLOT(setActiveViewerMode(Viewer::Mode const)));
   connect(&m_toolBar, SIGNAL(buildElementSelected(unsigned int)), 
      m_viewer, SLOT(setDefaultBuildElement(unsigned int)));
   connect(&m_toolBar, SIGNAL(buildFragmentSelected(QString const&, Viewer::Mode const)), 
      m_viewer, SLOT(setDefaultBuildFragment(QString const&, Viewer::Mode const)));
      

   connect(this, SIGNAL(recordingActive(bool)), 
      &m_toolBar, SLOT(setRecordAnimationButtonChecked(bool)));
         
   connect(this, SIGNAL(recordingActive(bool)), 
      m_viewer, SLOT(setRecord(bool)));

   connect(m_viewer, SIGNAL(recordingCanceled()), 
      this, SLOT(recordingCanceled()));
 

   // ViewerModel 
   connect(&m_viewerModel, SIGNAL(updated()), 
      m_viewer, SLOT(update()));

   connect(&m_viewerView, SIGNAL(doubleClicked(QModelIndex const&)),
      &m_viewerModel, SLOT(itemDoubleClicked(QModelIndex const&))); 

   connect(&m_viewerView, SIGNAL(expanded(QModelIndex const&)),
      &m_viewerModel, SLOT(itemExpanded(QModelIndex const&))); 

   connect(&m_viewerView, SIGNAL(newMoleculeFromSelection(QModelIndexList const&)),
      &m_viewerModel, SLOT(newMoleculeFromSelection(QModelIndexList const&)));

   connect(&m_viewerView, SIGNAL(mergeSelection(QModelIndexList const&)),
      &m_viewerModel, SLOT(mergeSelection(QModelIndexList const&)));

   connect(&m_viewerView, SIGNAL(deleteSelection(QModelIndexList const&)),
      &m_viewerModel, SLOT(deleteSelection(QModelIndexList const&)));

   connect(&m_viewerView, SIGNAL(showMolecules(QModelIndexList const&)),
      &m_viewerModel, SLOT(showMolecules(QModelIndexList const&)));

   connect(&m_viewerView, SIGNAL(hideMolecules(QModelIndexList const&)),
      &m_viewerModel, SLOT(hideMolecules(QModelIndexList const&)));


   connect(&m_viewerModel, SIGNAL(sceneRadiusChanged(double const)), 
      m_viewer, SLOT(setSceneRadius(double const)));

   connect(&m_viewerModel, SIGNAL(changeActiveViewerMode(Viewer::Mode const)), 
      m_viewer, SLOT(setActiveViewerMode(Viewer::Mode const)));

   connect(&m_viewerModel, SIGNAL(displayMessage(QString const&)),
       m_viewer, SLOT(displayMessage(QString const&)));

   connect(&m_viewerModel, SIGNAL(displayMessage(QString const&)),
       &m_statusWidget, SLOT(showMessage(QString const&)));

   connect(&m_viewerModel, SIGNAL(postCommand(QUndoCommand*)),
       this, SLOT(addCommand(QUndoCommand*)));

   connect(&m_viewerModel, SIGNAL(foregroundColorChanged(QColor const&)),
       m_viewer, SLOT(setForegroundColor(QColor const&)));

   connect(&m_viewerModel, SIGNAL(backgroundColorChanged(QColor const&)),
       m_viewer, SLOT(setBackgroundColor(QColor const&)));

   connect(&m_viewerModel, SIGNAL(pushAnimators(AnimatorList const&)),
      m_viewer, SLOT(pushAnimators(AnimatorList const&)));

   connect(&m_viewerModel, SIGNAL(popAnimators(AnimatorList const&)),
      m_viewer, SLOT(popAnimators(AnimatorList const&)));

   connect(&m_viewerModel, SIGNAL(fileOpened(QString const&)),
      this, SLOT(fileOpened(QString const&)));

   connect(&(Process::JobMonitor::instance()), 
       SIGNAL(resultsAvailable(QString const&, QString const&, void*)),
       &m_viewerModel, SLOT(open(QString const&, QString const&, void*)));


   // Viewer
   connect(m_viewer, SIGNAL(openFileFromDrop(QString const&)),
      &m_viewerModel, SLOT(open(QString const&)));

   connect(m_viewer, SIGNAL(enableUpdate(bool const)), 
      &m_viewerModel, SLOT(enableUpdate(bool const)));

   connect(m_viewer, SIGNAL(postCommand(QUndoCommand*)),
      this, SLOT(addCommand(QUndoCommand*)));

   connect(m_viewer, SIGNAL(activeViewerModeChanged(Viewer::Mode const)),
      &m_toolBar, SLOT(setToolBarMode(Viewer::Mode const)));

   connect(m_viewer, SIGNAL(escapeFullScreen()), 
      this, SLOT(fullScreen()));

   connect(m_viewer, SIGNAL(animationStep()), 
      &m_viewerModel, SLOT(reperceiveBondsForAnimation()));


   // Selection
   connect(&m_viewerSelectionModel, 
      SIGNAL(selectionChanged(QItemSelection const&, QItemSelection const&)),
      &m_viewerModel, SLOT(selectionChanged(QItemSelection const&, QItemSelection const&)) );

   connect(&m_viewerModel, 
      SIGNAL(selectionChanged(QItemSelection const&, QItemSelectionModel::SelectionFlags)),
      &m_viewerSelectionModel, 
      SLOT(select(QItemSelection const&, QItemSelectionModel::SelectionFlags)) );

   connect(&m_viewerModel, SIGNAL(clearSelection()),
      &m_viewerSelectionModel, SLOT(clearSelection()));

   connect(m_viewer, SIGNAL(clearSelection()),
      &m_viewerModel, SLOT(selectNone()));

   connect(m_viewer, 
      SIGNAL(select(QModelIndex const&, QItemSelectionModel::SelectionFlags)),
      &m_viewerSelectionModel, 
      SLOT(select(QModelIndex const&, QItemSelectionModel::SelectionFlags)));

   connect(&m_viewerModel, 
      SIGNAL(select(QModelIndex const&, QItemSelectionModel::SelectionFlags)),
      &m_viewerSelectionModel, 
      SLOT(select(QModelIndex const&, QItemSelectionModel::SelectionFlags)));
}




/******************
 *
 *  Event Handlers
 *
 ******************/

void MainWindow::resizeEvent(QResizeEvent* event)
{
   Preferences::MainWindowSize(event->size());
}


void MainWindow::closeEvent(QCloseEvent* event)
{
   if (!m_viewerModel.saveRequired()) {
      event->accept();
      return;
   }

   QPixmap pixmap;
   pixmap.load(":/imageQuestion");
   QMessageBox messageBox(QMessageBox::NoIcon, "IQmol", "Save Changes?");
   QPushButton* saveButton    = messageBox.addButton("Save", QMessageBox::AcceptRole);
   QPushButton* discardButton = messageBox.addButton("Discard", QMessageBox::RejectRole);
   QPushButton* cancelButton  = messageBox.addButton("Cancel", QMessageBox::RejectRole);
   messageBox.setIconPixmap(pixmap);
   messageBox.exec();

   if (messageBox.clickedButton() == saveButton) {
      if (m_viewerModel.saveAllAndClose()) {
         event->accept();
      }else {
         event->ignore();
      }
   }else if (messageBox.clickedButton() == discardButton) {
     event->accept();
   }else if (messageBox.clickedButton() == cancelButton) {
     event->ignore();
   }


}


void MainWindow::splitterMoved(int pos, int)
{
   Preferences::ShowModelView(pos != 0);
}




/******************
 *
 *  Initialization 
 *
 ******************/

void MainWindow::createMenus()
{
   QAction* action;
   QString  name;
   QMenu*   menu;
   QMenu*   subMenu;

   // ----- File Menu -----
   menu = menuBar()->addMenu("File");

      name = "About";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showAbout()));

      name = "New Molecule";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(newMoleculeMenu()));
      action->setShortcut(QKeySequence::New);

      name = "New Viewer";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(newViewer()));

      name = "Open";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(openFile()));
      action->setShortcut(QKeySequence::Open);

      name = "Open Dir";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(openDir()));
      action->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_O);

      name = "Open Recent";
      m_recentFilesMenu = menu->addMenu(name);
      updateRecentFilesMenu();

      menu->addSeparator();

      name = "Close Viewer";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(close()));
      action->setShortcut(QKeySequence::Close);

      name = "Save";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(saveAll()));
      action->setShortcut(QKeySequence::Save);

      name = "Save As";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(saveAs()));
      action->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_S);

      menu->addSeparator();

      name = "Save Picture";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), m_viewer, SLOT(takeSnapshot()));
      action->setShortcut(Qt::CTRL | Qt::Key_P);

/*    This is now handled by the POV-Ray dialog
      name = "Generate PovRay Input";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(generatePovRay()));
*/

      name = "Record Animation";
      action = menu->addAction(name);
      action->setCheckable(true);
      action->setChecked(false);
      connect(action, SIGNAL(triggered()), this, SLOT(toggleRecordingActive()));
      action->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_P);
      m_recordAnimationAction = action;

      name = "Show Message Log";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showLogMessages()));
      action->setShortcut(Qt::CTRL | Qt::Key_L);

      name = "Quit";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SIGNAL(quit()));
      action->setShortcut(Qt::CTRL | Qt::Key_Q);


   // ----- Edit Menu -----
   menu = menuBar()->addMenu("Edit");
  
      name = "Undo";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_undoStack, SLOT(undo()));
      action->setShortcut(Qt::CTRL | Qt::Key_Z); 
      connect(&m_undoStack, SIGNAL(canUndoChanged(bool)), action, SLOT(setEnabled(bool)));

      name = "Redo";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_undoStack, SLOT(redo()));
      action->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_Z); 
      connect(&m_undoStack, SIGNAL(canRedoChanged(bool)), action, SLOT(setEnabled(bool)));

      menu->addSeparator();

      name = "Cut";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(cutSelection()));
      action->setShortcut(Qt::CTRL | Qt::Key_X);

      name = "Copy";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(copySelectionToClipboard()));
      action->setShortcut(Qt::CTRL | Qt::Key_C);

      name = "Paste";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(pasteSelectionFromClipboard()));
      action->setShortcut(Qt::CTRL | Qt::Key_V);

      menu->addSeparator();

      name = "Select All";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(selectAll()));
      action->setShortcut(Qt::CTRL | Qt::Key_A);

      name = "Select None";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SIGNAL(clearSelection()));
      action->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_A);

      name = "Invert Selection";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(invertSelection()));
      action->setShortcut(Qt::CTRL | Qt::Key_I);

      menu->addSeparator();

      name = "Reindex Atoms";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(reindexAtoms()));


      menu->addSeparator();

      name = "Preferences";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showPreferences()));


   // ----- Display Menu -----
   menu = menuBar()->addMenu("Display");

      name = "Full Screen";
      action = menu->addAction(name);
      action->setCheckable(true);
      action->setChecked(false);
      connect(action, SIGNAL(triggered()), this, SLOT(fullScreen()));
      action->setShortcut(Qt::CTRL | Qt::Key_0);
      m_fullScreenAction = action;

      name = "Reset View";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), m_viewer, SLOT(resetView()));
      action->setShortcut(Qt::CTRL | Qt::Key_R);

      name = "Show Axes";
      action = menu->addAction(name);
      action->setCheckable(true);
      action->setChecked(false);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(toggleAxes()));
      connect(&m_viewerModel, SIGNAL(axesOn(bool)), action, SLOT(setChecked(bool)));
      action->setShortcut(Qt::Key_A);


      menu->addSeparator();

      name = "Camera";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(configureCamera()));

      name = "Appearance";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(configureAppearance()));

      menu->addSeparator();

      name = "Atom Labels";
      subMenu = menu->addMenu(name);

         name = "Element";
         action = subMenu->addAction(name);
         connect(action, SIGNAL(triggered()), this, SLOT(setLabel()));
         action->setData(Layer::Atom::Element);
         action->setShortcut(Qt::Key_E);
         action->setCheckable(true);
         m_labelActions << action;

         name = "Index";
         action = subMenu->addAction(name);
         connect(action, SIGNAL(triggered()), this, SLOT(setLabel()));
         action->setData(Layer::Atom::Index);
         action->setShortcut(Qt::Key_I);
         action->setCheckable(true);
         m_labelActions << action;
   
         name = "Mass";
         action = subMenu->addAction(name);
         connect(action, SIGNAL(triggered()), this, SLOT(setLabel()));
         action->setData(Layer::Atom::Mass);
         action->setShortcut(Qt::Key_M);
         action->setCheckable(true);
         m_labelActions << action;

         name = "NMR";
         action = subMenu->addAction(name);
         connect(action, SIGNAL(triggered()), this, SLOT(setLabel()));
         action->setData(Layer::Atom::NmrShift);
         action->setShortcut(Qt::Key_N);
         action->setCheckable(true);
         m_labelActions << action;

         name = "Partial Charge";
         action = subMenu->addAction(name);
         connect(action, SIGNAL(triggered()), this, SLOT(setLabel()));
         action->setData(Layer::Atom::Charge);
         action->setShortcut(Qt::Key_Q);
         action->setCheckable(true);
         m_labelActions << action;

         name = "Spin Densities";
         action = subMenu->addAction(name);
         connect(action, SIGNAL(triggered()), this, SLOT(setLabel()));
         action->setData(Layer::Atom::Spin);
         action->setShortcut(Qt::Key_S);
         action->setCheckable(true);
         m_labelActions << action;

         name = "Atom Label";
         action = subMenu->addAction(name);
         connect(action, SIGNAL(triggered()), this, SLOT(setLabel()));
         action->setData(Layer::Atom::Label);
         action->setShortcut(Qt::Key_L);
         action->setCheckable(true);
         m_labelActions << action;


   // ----- Build Menu -----
   menu = menuBar()->addMenu("Build");

      name = "Insert Molecule by ID";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(insertMoleculeDialog()));
      action->setShortcut(Qt::CTRL | Qt::Key_B);

      menu->addSeparator();

      name = "Fill Valencies With Hydrogens";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(addHydrogens()));
      action->setShortcut(Qt::CTRL | Qt::Key_F);

      name = "Reperceive Bonds";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(reperceiveBonds()));

      name = "Set Isotopes";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(setIsotopes()));

      menu->addSeparator();

      name = "Set Geometric Constraint";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(setConstraint()));
      action->setShortcut(Qt::CTRL | Qt::Key_K);

      name = "Freeze Selected Atoms";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(freezeAtomPositions()));

      name = "Minimize Structure";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(minimizeEnergy()));
      action->setShortcut(Qt::CTRL | Qt::Key_M);

      name = "Select Force Field";

      QActionGroup* forceFieldGroup = new QActionGroup(this);
      subMenu = menu->addMenu(name);

         QString ff("MMFF94");
         action = subMenu->addAction(ff);
         action->setCheckable(true);
         action->setData(ff);
         forceFieldGroup->addAction(action);
         connect(action, SIGNAL(triggered()), this, SLOT(setForceField()));
         if (Preferences::DefaultForceField() == ff) action->setChecked(true);

         ff = "MMFF94s";
         action = subMenu->addAction(ff);
         action->setCheckable(true);
         action->setData(ff);
         forceFieldGroup->addAction(action);
         connect(action, SIGNAL(triggered()), this, SLOT(setForceField()));
         if (Preferences::DefaultForceField() == ff) action->setChecked(true);

         ff = "UFF";
         action = subMenu->addAction(ff);
         action->setCheckable(true);
         action->setData(ff);
         forceFieldGroup->addAction(action);
         connect(action, SIGNAL(triggered()), this, SLOT(setForceField()));
         if (Preferences::DefaultForceField() == ff) action->setChecked(true);

         ff = "Ghemical";
         action = subMenu->addAction(ff);
         action->setCheckable(true);
         action->setData(ff);
         forceFieldGroup->addAction(action);
         connect(action, SIGNAL(triggered()), this, SLOT(setForceField()));
         if (Preferences::DefaultForceField() == ff) action->setChecked(true);

         ff = "Gaff";
         action = subMenu->addAction(ff);
         action->setCheckable(true);
         action->setData(ff);
         forceFieldGroup->addAction(action);
         connect(action, SIGNAL(triggered()), this, SLOT(setForceField()));
         if (Preferences::DefaultForceField() == ff) action->setChecked(true);

      menu->addSeparator();

      name = "Translate To Center";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(translateToCenter()));
      action->setShortcut(Qt::CTRL | Qt::Key_T );

      name = "Symmetrize Molecule";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(symmetrize()));
      action->setShortcut(Qt::CTRL | Qt::Key_Y );

      name = "Set Symmetry Tolerance";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(adjustSymmetryTolerance()));

      name = "Auto-detect Symmetry";
      action = menu->addAction(name);
      action->setCheckable(true);
      action->setChecked(false);
      connect(action, SIGNAL(triggered()), &m_viewerModel, SLOT(toggleAutoDetectSymmetry()));



   // ----- Calculation Menu -----
   menu = menuBar()->addMenu("Calculation");

      name = "Q-Chem Setup";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showQChemUI()));
      action->setShortcut(Qt::CTRL | Qt::Key_U );
      m_qchemSetupAction = action;

      name = "Job Monitor";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showJobMonitor()));
      action->setShortcut(Qt::CTRL | Qt::Key_J );

      name = "Edit Servers";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(editNewServers()));

      //name = "Test Internet Connection";
      //action = menu->addAction(name);
      //connect(action, SIGNAL(triggered()), this, SLOT(testInternetConnection()));

#ifdef GROMACS
      menu->addSeparator();
      name = "Gromacs Setup";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showGromacsDialog()));
      action->setShortcut(Qt::CTRL | Qt::Key_G );

      name = "Edit Gromacs Config";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showGromacsConfigDialog()));

      name = "Edit Gomacs Server";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showGromacsServerDialog()));



#endif

#ifdef AMBER
   // ----- Amber Menu -----
   if (Preferences::AmberEnabled()) {
      menu->addSeparator();
      name = "Edit Amber Config";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showAmberConfigDialog()));

      name = "Amber System Builder";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showAmberSystemBuilderDialog()));
      connect(menu, &QMenu::aboutToShow, [this, action](){
         action->setEnabled(m_viewerModel.activeSystem() != 0);
      });
   }
#endif

   // ----- Help Menu -----
   menu = menuBar()->addMenu("Help");

      name = "Show Help";
      action = menu->addAction(name);
      connect(action, SIGNAL(triggered()), this, SLOT(showHelp()));
}


void MainWindow::editNewServers()
{
   Process::ServerConfigurationListDialog dialog(this);
   dialog.exec();  
}


void MainWindow::showLogMessages()
{ 
   if (m_logMessageDialog.isActive()) {
      m_logMessageDialog.close(); 
   }else {
      m_logMessageDialog.display(); 
   }
}


void MainWindow::showJobMonitor() { 
   Process::JobMonitor::instance().show(); 
   Process::JobMonitor::instance().raise();
}


void MainWindow::testInternetConnection()
{
   if (Network::TestNetworkConnection()) {
      QMsgBox::information(this, "IQmol", "Network access available");
   }
}


void MainWindow::configureCamera()
{
   if (m_viewer) m_viewer->editCamera();
}


void MainWindow::configureAppearance()
{
   if (m_viewer) m_viewer->editShaders();
}


void MainWindow::newViewer()
{
   MainWindow* mw(new MainWindow());
   mw->move(x()+30,y()+30);
   mw->show();
}


void MainWindow::openFile()
{
   QString fileName = FileDialog::getOpenFileName(this, tr("Open File"), 
      Preferences::LastFileAccessed());
   if (!fileName.isEmpty()) {
      m_statusWidget.showMessage("Loading", true);
      open(fileName);
   }
}


void MainWindow::openDir()
{
   QString dirName = FileDialog::getExistingDirectory(this, tr("Open Job Directory"), 
      Preferences::LastFileAccessed());
   if (!dirName.isEmpty()) {
      m_statusWidget.showMessage("Loading", true);
      open(dirName);
   }
}


void MainWindow::openRecentFile()
{
   QAction* action = qobject_cast<QAction*>(sender());
   if (action) {
      QString name(action->data().toString());
      m_statusWidget.showMessage("Loading", true);
      open(name);
   }
}


void MainWindow::fileOpened(QString const& filePath)
{
   Preferences::AddRecentFile(filePath);
   Preferences::LastFileAccessed(filePath);
   updateRecentFilesMenu();
   m_statusWidget.clearMessage();
}


void MainWindow::reindexAtoms()
{
   bool visibleOnly(true); // we only want the visible molecules;
   MoleculeList molecules(m_viewerModel.moleculeList(visibleOnly));

   if (molecules.size() == 0) return;
   if (molecules.size() > 1) {
      QString msg("Only one molecule can be visible when reindexing atoms.");
      QMsgBox::warning(this, "IQmol", msg);
      return;
   }

   QList<QAction*>::iterator iter;
   for (iter = m_labelActions.begin(); iter != m_labelActions.end(); ++iter) {
       (*iter)->setChecked(false); 
   }

   m_viewer->reindexAtoms(molecules.first());
}


void MainWindow::toggleRecordingActive()
{
   setRecord(m_recordAnimationAction->isChecked());
}


void MainWindow::setRecord(bool active)
{
   // Synchronizes the menu and ToolBar
   m_recordAnimationAction->setChecked(active);
   recordingActive(active);
}


void MainWindow::setLabel()
{
   QAction* action(qobject_cast<QAction*>(sender()));

   bool turnOn(action->isChecked());

   if (turnOn) {
      m_viewer->setLabelType(action->data().toInt());
   }else {
      m_viewer->setLabelType(Layer::Atom::None);
   }

   QList<QAction*>::iterator iter;
   for (iter = m_labelActions.begin(); iter != m_labelActions.end(); ++iter) {
       (*iter)->setChecked(false);
   }
   if (turnOn) action->setChecked(true);
}



void MainWindow::setForceField()
{
   QAction* action = qobject_cast<QAction*>(sender());
   if (action) m_viewerModel.setForceField(action->data().toString());
}


void MainWindow::updateRecentFilesMenu()
{     
   if (m_recentFilesMenu) {
      m_recentFilesMenu->clear();
      QStringList files(Preferences::RecentFiles());
      QAction* action;
      QFileInfo info;
      
      for (int i = 0; i < files.count(); ++i) {
          info.setFile(files[i]);
          if (info.exists() && info.isReadable()) {
             action = m_recentFilesMenu->addAction(info.fileName());
             action->setData(files[i]);
             connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
          }else {
             Preferences::RemoveRecentFile(files[i]);
          }
      }
      
      m_recentFilesMenu->addSeparator();
      action = m_recentFilesMenu->addAction("Clear List");
      connect(action, SIGNAL(triggered()), this, SLOT(clearRecentFilesMenu()));
   }
}


void MainWindow::clearRecentFilesMenu()
{
   Preferences::ClearRecentFiles();
   updateRecentFilesMenu();
}


void MainWindow::fullScreen()
{
   QString msg;

   if (m_viewer->isFullScreen()) {
      // From full screen
      m_toolBar.show();
      m_sideSplitter->show();
      m_viewer->setFullScreen(false);
      m_fullScreenAction->setChecked(false);
      msg = "";
   } else {
      // To full screen
      m_toolBar.hide();
      m_sideSplitter->hide();
      m_viewer->setFullScreen(true);
      m_fullScreenAction->setChecked(true);
      msg = "Use <esc> to exit full screen mode";
   }

   m_viewer->QGLViewer::displayMessage(msg, 5000);
}


void MainWindow::showQChemUI() 
{
   if (!m_quiInputDialog) {
      m_quiInputDialog = new Qui::InputDialog(this);
      if (!m_quiInputDialog->init()) {
         m_qchemSetupAction->setEnabled(false);
         delete m_quiInputDialog;
         m_quiInputDialog = 0;
         return;
      }

      connect(m_quiInputDialog, SIGNAL(submitJobRequest(IQmol::Process::JobInfo&)),
         this, SLOT(submitJob(IQmol::Process::JobInfo&)));
 
 
      connect(&(Process::JobMonitor::instance()), SIGNAL(jobAccepted()),
         m_quiInputDialog, SLOT(closeDialog()));

      connect(&(Process::JobMonitor::instance()), SIGNAL(postUpdateMessage(QString const&)),
         m_quiInputDialog, SLOT(showMessage(QString const&)));
   }

   Layer::Component* comp(m_viewerModel.activeComponent());
   if (!comp) return;
   
   if (!comp->sanityCheck()) {
      QMessageBox mbox;
      mbox.setText("Wonky molecule detected");
      mbox.setInformativeText("Do you want to proceed?");
      mbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
      mbox.setDefaultButton(QMessageBox::Ok);
   
      QPixmap pixmap;
      pixmap.load(":/imageQuestion");
      mbox.setIconPixmap(pixmap);

      if (mbox.exec() == QMessageBox::Cancel) return;
   }

   Process::JobInfo jobInfo(comp->qchemJobInfo());

   m_quiInputDialog->setJobInfo(jobInfo);

   // (Re-)Load the servers here in case the user has made any modifications
   QStringList serverList(Process::ServerRegistry::instance().availableServers());
   if (serverList.isEmpty()) serverList.append("(none)");
   m_quiInputDialog->setServerList(serverList);

   m_viewer->setActiveViewerMode(Viewer::Manipulate);

   m_quiInputDialog->showMessage("");
   m_quiInputDialog->setWindowModality(Qt::WindowModal);
   m_quiInputDialog->show();
}


void MainWindow::submitJob(IQmol::Process::JobInfo &jobInfo)
{
   QLOG_DEBUG() << "Entered submitJob";
   Process::JobMonitor::instance().submitJob(&jobInfo);
   Layer::Molecule* mol(m_viewerModel.activeMolecule());
   if (!mol) return;
   mol->jobInfoChanged(jobInfo);
}


void MainWindow::showGromacsDialog() 
{
#ifdef GROMACS
   if (!m_gromacsDialog) {
      m_gromacsDialog = new Gmx::GromacsDialog(this);


      connect(m_gromacsDialog, SIGNAL(submitGromacsJobRequest(IQmol::Process::JobInfo&)),
         this, SLOT(submitJob(IQmol::Process::JobInfo&)));
   }

   Layer::System* sys(m_viewerModel.activeSystem());
   if (!sys) return;
   
   //m_gromacsDialog->setWindowModality(Qt::WindowModal);
   m_gromacsDialog->show();
   m_gromacsDialog->raise();
#endif
}

void MainWindow::showGromacsConfigDialog()
{
#ifdef GROMACS
   Gmx::GromacsConfigDialog dialog(this);
   dialog.exec();
   if (dialog.result() == QDialog::Accepted) {
      Preferences::GromacsTopologyFile(dialog.getTopology());
      Preferences::GromacsPositionsFile(dialog.getPositions());

   }
   
#endif
}


void MainWindow::showGromacsServerDialog()
{
#ifdef GROMACS
   Gmx::GromacsServerDialog dialog(this);
   dialog.exec();
   if (dialog.result() == QDialog::Accepted) {
      Preferences::GromacsServerAddress(dialog.getAddress());
   }
#endif
}


void MainWindow::showAmberConfigDialog()
{
#ifdef AMBER
   Amber::ConfigDialog dialog(this);
   dialog.exec();
   if (dialog.result() == QDialog::Accepted) {
      Preferences::AmberDirectory(dialog.getDirectory());
   }
#endif
}


void MainWindow::showAmberSystemBuilderDialog()
{
#ifdef AMBER
   Amber::SystemBuilderDialog dialog(this, m_viewerModel.activeSystem());
   dialog.exec();
#endif
}

void MainWindow::insertMoleculeDialog() 
{
   InsertMoleculeDialog dialog(this);
   connect( &dialog, SIGNAL(insertMoleculeById(QString)), 
       &m_viewerModel, SLOT(insertMoleculeById(QString)) );
   dialog.exec();
}

} // end namespace IQmol
