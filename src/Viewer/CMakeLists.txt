set(LIB Viewer)

find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)

set( HEADERS
   Animator.h
   CameraDialog.h
   ShaderDialog.h
   Snapshot.h
   SymmetryToleranceDialog.h
   Viewer.h
   ViewerModel.h
   ViewerModelView.h
)

set( SOURCES
   Animator.C
   BuildAtomHandler.C
   BuildEfpFragmentHandler.C
   BuildFunctionalGroupHandler.C
   BuildHandler.C
   BuildMoleculeFragmentHandler.C
   CameraDialog.C
   Cursors.C
   GLSLmath.C
   gl2ps.C
   ManipulateHandler.C
   ManipulateSelectionHandler.C
   ManipulatedFrameSetConstraint.C
   PovRayGen.C
   ReindexAtomsHandler.C
   SelectHandler.C
   ShaderDialog.C
   ShaderLibrary.C
   Snapshot.C
   SymmetryToleranceDialog.C
   UndoCommands.C
   Viewer.C
   ViewerModel.C
   ViewerModelView.C
)

set( UI_FILES
   CameraDialog.ui
   ShaderDialog.ui
   SymmetryToleranceDialog.ui
   Snapshot.ui
)

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES} ${UI_HEADERS})

target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${LIB} PUBLIC
   Configurator
)

target_link_libraries(${LIB} PRIVATE
   Parser # TextStream
   Process # ServerConfiguration
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)
