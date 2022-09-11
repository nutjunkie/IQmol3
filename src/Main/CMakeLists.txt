find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)
set(LIB Main)

set( HEADERS
   AboutDialog.h
   FragmentTable.h
   HelpBrowser.h
   IQmolApplication.h
   InsertMoleculeDialog.h
   MainWindow.h
   PeriodicTable.h
   PreferencesBrowser.h
   ToolBar.h
   version.h
)

set( SOURCES
   AboutDialog.C
   FragmentTable.C
   HelpBrowser.C
   IQmolApplication.C
   InsertMoleculeDialog.C
   MainWindow.C
   PeriodicTable.C
   PreferencesBrowser.C
   ToolBar.C
)

set( UI_FILES
   AboutDialog.ui
   FragmentTable.ui
   HelpBrowser.ui
   InsertMoleculeDialog.ui
   PeriodicTable.ui
   PeriodicTableMac.ui
   PreferencesBrowser.ui
   ToolBar.ui
   ../Process/JobMonitor.ui
   ../Process/ServerConfigurationListDialog.ui
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ../Process)

#include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.. ../Viewer ../Layer ../Configurator ../Util ../Data ../Old ../Process ../Network)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Configurator)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Old)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Qui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Process)

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES} ${UI_FILES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)
