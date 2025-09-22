#pragma once
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

#include <QFileInfo>
#include <QStatusBar>

#include "Logic.h"
#include "OptionRegister.h"
#include "ui_InputDialog.h"
#include "JobInfo.h"

#include "ui_AdcTab.h"
#include "ui_AimdTab.h"
#include "ui_AttenuationParameterTab.h"
#include "ui_AuxiliaryBasisTab.h"
#include "ui_CisTab.h"
#include "ui_EomTab.h"
#include "ui_FreezingStringTab.h"
#include "ui_FrequenciesTab.h"
#include "ui_GeometryTab.h"
#include "ui_PrimaryBasisTab.h"
#include "ui_PropertiesTab.h"
#include "ui_ReactionPathTab.h"
#include "ui_TransitionStateTab.h"
#include "ui_OptimizeTab.h"
#include "ui_Libopt3Tab.h"
#include "Tab.h"

class QResizeEvent;
class QStackedWidget;
class QFont;


namespace Qui {

class Job;
class QtNode;
class Option;
class Molecule;
class OptionDatabase;

template<class K, class T> class Register;

typedef std::function<void(QString const&)> Update;
typedef QList<Job*> JobList;


/// \brief This is the main class for the QChem input file generator.  If you
/// think this class is bloated, it is because it is.  Further member functions
/// can be found in InputDialogLogic.C and InputDialogSlots.C
class InputDialog : public QMainWindow {

   Q_OBJECT

   public:
      InputDialog(QWidget* parent = 0);
      ~InputDialog();

      /// Returns true only if the option database is loaded 
      /// correctly and the InputDialog can be used. 
      bool init();

	  /// The JobInfo object encapsulates the communication 
	  /// between the QUI and IQmol.
      void setJobInfo(IQmol::Process::JobInfo const&);

      /// Allows the update of the servers in the server ComboBox
      void setServerList(QStringList const& servers);

   public Q_SLOTS:
      void showMessage(QString const& msg);
      void closeDialog();

   Q_SIGNALS:
      void submitJobRequest(IQmol::Process::JobInfo&);


   private Q_SLOTS:
      void on_resetButton_clicked(bool) { resetInput(); }
      void on_advancedOptionsTree_itemClicked(QTreeWidgetItem* item, int col);
      void on_jobList_currentIndexChanged(int);
      void on_previewText_textChanged() { setTaint(true, 0); }
      void on_editJobSectionButton_clicked();
      void on_qui_charge_valueChanged(int);
      void on_qui_multiplicity_valueChanged(int);
      void on_addJobButton_clicked(bool) { addNewJob(); }
      void on_deleteJobButton_clicked(bool); 
      void on_submitButton_clicked(bool) { submitJob(); }
      void on_disableControlsButton_clicked(bool);

      // Radio toggles for switching pages on stacked widgets
      void toggleStack(QStackedWidget* stack, bool on, QString model);
      void on_qui_use_case_toggled(bool);
      void on_ftc_toggled(bool);
      void on_qui_cfmm_toggled(bool);
      void on_qui_optimize_toggled(bool);
      void on_qui_libopt3_toggled(bool);
      void on_qui_use_ri_toggled(bool);
      void on_qui_none_toggled(bool on);

      void on_solvent_method_currentTextChanged(QString const&);

      void on_readChargesButton_clicked(bool);

      // Manual slots
      void widgetChanged(QObject* orig, QString const& value);
      void widgetChanged(QString const& value);
      void widgetChanged(int const& value);
      void widgetChanged(bool const& value);

      void changeComboBox(QString const& name, QString const& value);
      void changeCheckBox(QString const& name, QString const& value);
      void changeSpinBox(QString const& name, QString const& value);
      void changeDoubleSpinBox(QString const& name, QString const& value);
      void changeLineEdit(QString const& name, QString const& value);
      void changeRadioButton(QString const& name, QString const& value);

      void updatePreviewText() { updatePreviewText(m_jobs, m_currentJob); }

      // Menu Slots
      void menuSave()   { saveFile(false);  }
      void menuSaveAs() { saveFile(true);  }
      void addNewJob();
      void resetInput();
      void submitJob();
      void fontBigger()  { fontAdjust(true);  }
      void fontSmaller() { fontAdjust(false); }
      void setFont();
      void readChargesFinished();


   protected:
      void resizeEvent(QResizeEvent* event);

   private:
      Ui::MainWindow  m_ui;

      Tab<Ui::AdcTab>                  m_adcTab;
      Tab<Ui::AimdTab>                 m_aimdTab;
      Tab<Ui::AttenuationParameterTab> m_attenuationParameterTab;
      Tab<Ui::AuxiliaryBasisTab>       m_auxiliaryBasisTab;
      Tab<Ui::CisTab>                  m_cisTab;
      Tab<Ui::EomTab>                  m_eomTab;
      Tab<Ui::FreezingStringTab>       m_freezingStringTab;
      Tab<Ui::FrequenciesTab>          m_frequenciesTab;
      Tab<Ui::GeometryTab>             m_geometryTab;
      Tab<Ui::PrimaryBasisTab>         m_primaryBasisTab;
      Tab<Ui::PropertiesTab>           m_propertiesTab;
      Tab<Ui::ReactionPathTab>         m_reactionPathTab;
      Tab<Ui::TransitionStateTab>      m_transitionStateTab;

      Tab<Ui::OptimizeTab>             m_optimizeTab;
      Tab<Ui::Libopt3Tab>              m_libopt3Tab;

      IQmol::Process::JobInfo m_qchemJobInfo;

      OptionDatabase& m_db;
      OptionRegister& m_reg;
      bool m_taint;

      Job* m_currentJob;
      JobList m_jobs;
      std::vector<Action*> m_resetActions;
      std::map<QString,Update*> m_setUpdates;
      QFileInfo m_fileIn;
      QStatusBar m_statusBar;

      QMap<QString, QWidget*> m_toolBoxOptions;


      // ---------- Functions ----------

      void finalizeJob(Job*);
      void addJobToList(Job*);
      void removeJobFromList(Job*);
      void deleteAllJobs(bool const prompt = true);

      void capturePreviewTextChanges();
      void loadPreviewText(QString const& text);
      void updatePreviewText(JobList const&, Job const* currentJob);

      void setControls(Job* job);
      void resetControls();
      void initializeMenus();
      bool initializeControls();
      void initializeQuiLogic();
      void initializeStackedWidgets();

      void initializeControl(Option const& opt, QComboBox* combo);
      void initializeControl(Option const& opt, QCheckBox* check);
      void initializeControl(Option const& opt, QLineEdit* edit);
      void initializeControl(Option const& opt, QSpinBox*  spin);
      void initializeControl(Option const& opt, QDoubleSpinBox* dspin);
      void initializeControl(Option const& opt, QRadioButton* radio);

      void connectControl(Option const& opt, QComboBox* combo);
      void connectControl(Option const& opt, QCheckBox* check);
      void connectControl(Option const& opt, QLineEdit* edit);
      void connectControl(Option const& opt, QSpinBox* spin);
      void connectControl(Option const& opt, QRadioButton* radio);
      void connectControl(Option const& opt, QDoubleSpinBox* dspin);

      void widgetError(QString const& name);
      void printSection(QString const& name, bool doPrint);
      void printOption(QString const& name, bool doPrint);
      void printOptionDebug(QString const& name, bool doPrint);
      void updateLJParameters();
      bool saveFile(bool prompt);

      void fontAdjust(bool makeBigger);
      void changePreviewFont(QFont const& font);
      void setTaint(bool const tf, int line);

      QStringList generateInputJobStrings(JobList const&, bool preview);
      QString generateInputString();

      QString prependRemName(QString const& name, QString const& description);
};

} // end namespace Qui
