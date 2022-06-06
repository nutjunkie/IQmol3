set(LIB Qui)
find_package(Qt5 COMPONENTS Core Gui Widgets Sql REQUIRED)
find_package(Qt5 COMPONENTS Xml OpenGL REQUIRED)

set( HEADERS
   Actions.h
   AdcTab.h
   AimdTab.h
   AttenuationParameterTab.h
   AuxiliaryBasisTab.h
   CisTab.h
   Conditions.h
   EomTab.h
   ExternalChargesSection.h
   FreezingStringTab.h
   FrequenciesTab.h
   GeometryConstraint.h
   InputDialog.h
   KeyValueSection.h
   KeywordSection.h
   LJParametersSection.h
   Logic.h
   MoleculeSection.h
   Node.h
   OptSection.h
   Option.h
   OptionDatabase.h
   OptionDatabaseForm.h
   OptionEditors.h
   OptionRegister.h
   PcmSection.h
   PrimaryBasisTab.h
   PropertiesTab.h
   QtNode.h
   Qui.h
   QuiJob.h
   QuiMolecule.h
   ReactionPathTab.h
   Register.h
   RemSection.h
   TransitionStateTab.h
)

set( SOURCES
   Actions.C
   ExternalChargesSection.C
   GeometryConstraint.C
   InputDialog.C
   InputDialogLogic.C
   KeyValueSection.C
   KeywordSection.C
   LJParametersSection.C
   MoleculeSection.C
   OptSection.C
   Option.C
   OptionDatabase.C
   OptionDatabaseForm.C
   OptionEditors.C
   PcmSection.C
   Qui.C
   QuiJob.C
   QuiMolecule.C
   ReadInput.C
   RemSection.C
)

set( UI_FILES
   AdcTab.ui
   AimdTab.ui
   AttenuationParameterTab.ui
   AuxiliaryBasisTab.ui
   CisTab.ui
   EomTab.ui
   FreezingStringTab.ui
   FrequenciesTab.ui
   GeometryConstraintDialog.ui
   OptionDatabaseForm.ui
   OptionListEditor.ui
   OptionNumberEditor.ui
   PrimaryBasisTab.ui
   PropertiesTab.ui
   QuiMainWindow.ui
   ReactionPathTab.ui
   TransitionStateTab.ui
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../Process ../Util ../Data ../ ../Parser)

qt5_wrap_ui( SOURCES ${UI_FILES} )
qt5_wrap_cpp( SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::Sql
   Qt5::OpenGL
)

