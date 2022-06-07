find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)
set(LIB Old)

set( HEADERS
   AtomicDensity.h
   AtomicDensityData.h
   ColorGrid.h
   LogMessageDialog.h
   ParseJobFiles.h
   ProgressDialog.h
   SpatialProperty.h
   SymmetryToleranceDialog.h
   UndoCommands.h
   gl2ps.h
)

set( SOURCES
   AtomicDensity.C
   ColorGrid.C
   LogMessageDialog.C
   ParseJobFiles.C
   ProgressDialog.C
   SpatialProperty.C
   SymmetryToleranceDialog.C
   UndoCommands.C
   gl2ps.C
)

set( UI_FILES
   GetVaultKeyDialog.ui
   LogMessageDialog.ui
   ProgressDialog.ui
   SymmetryToleranceDialog.ui
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

include_directories(.. ../Util ../Data ../Parser ../Layer ../Configurator ../Viewer)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Configurator)

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
