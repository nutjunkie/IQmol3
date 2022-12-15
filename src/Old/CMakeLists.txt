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


qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} ${SOURCES} ${UI_HEADERS})
target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(${LIB} PRIVATE
   "${OpenBabel3_INCLUDE_DIRS}"
)
target_link_libraries(${LIB} PUBLIC
   Parser
)
target_link_libraries(${LIB} PRIVATE
   Configurator
   Data
   Util
   ${OpenBabel3_LIBRARIES}
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
   ZLIB::ZLIB
)
