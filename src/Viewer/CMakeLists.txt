set(LIB Viewer)

find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)

set( HEADERS
   Animator.h
   BaseHandler.h
   BuildAtomHandler.h
   BuildEfpFragmentHandler.h
   BuildFunctionalGroupHandler.h
   BuildHandler.h
   BuildMoleculeFragmentHandler.h
   CameraDialog.h
   Cursors.h
   GLSLmath.h
   ManipulateHandler.h
   ManipulateSelectionHandler.h
   ManipulatedFrameSetConstraint.h
   PovRayGen.h
   ReindexAtomsHandler.h
   SelectHandler.h
   ShaderDialog.h
   ShaderLibrary.h
   Snapshot.h
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
   ManipulateHandler.C
   ManipulateSelectionHandler.C
   ManipulatedFrameSetConstraint.C
   PovRayGen.C
   ReindexAtomsHandler.C
   SelectHandler.C
   ShaderDialog.C
   ShaderLibrary.C
   Snapshot.C
   Viewer.C
   ViewerModel.C
   ViewerModelView.C
)

set( UI_FILES
   CameraDialog.ui
   ShaderDialog.ui
   Snapshot.ui
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(../Layer ../Configurator ../Util .. ../Data ../Old ../Parser ../Yaml ../Process ../Network)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Process)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Configurator)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Old)

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
