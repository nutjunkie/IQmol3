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

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} ${SOURCES} ${UI_FILES})
target_include_directories(${LIB} PUBLIC
   "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(${LIB} PUBLIC
   Viewer
)
target_link_libraries(${LIB} PRIVATE
   Process
   Qui
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)

if (WITH_GROMACS)
   target_link_libraries(${LIB} PRIVATE Gromacs)
endif()

target_link_libraries(${LIB} PRIVATE Amber)
