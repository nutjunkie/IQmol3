find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL Xml REQUIRED)
set(LIB QGLViewer)

set( HEADERS
   libQGLViewer//QGLViewer/VRender/Primitive.h
   libQGLViewer//QGLViewer/VRender/Vector2.h
   libQGLViewer//QGLViewer/VRender/Types.h
   libQGLViewer//QGLViewer/VRender/AxisAlignedBox.h
   libQGLViewer//QGLViewer/VRender/PrimitivePositioning.h
   libQGLViewer//QGLViewer/VRender/VRender.h
   libQGLViewer//QGLViewer/VRender/SortMethod.h
   libQGLViewer//QGLViewer/VRender/Optimizer.h
   libQGLViewer//QGLViewer/VRender/gpc.h
   libQGLViewer//QGLViewer/VRender/NVector3.h
   libQGLViewer//QGLViewer/VRender/ParserGL.h
   libQGLViewer//QGLViewer/VRender/Exporter.h
   libQGLViewer//QGLViewer/VRender/Vector3.h
)

set( SOURCES
   libQGLViewer/QGLViewer/qglviewer.cpp
   libQGLViewer/QGLViewer/camera.cpp
   libQGLViewer/QGLViewer/manipulatedFrame.cpp
   libQGLViewer/QGLViewer/manipulatedCameraFrame.cpp
   libQGLViewer/QGLViewer/frame.cpp
   libQGLViewer/QGLViewer/saveSnapshot.cpp
   libQGLViewer/QGLViewer/constraint.cpp
   libQGLViewer/QGLViewer/keyFrameInterpolator.cpp
   libQGLViewer/QGLViewer/mouseGrabber.cpp
   libQGLViewer/QGLViewer/quaternion.cpp
   libQGLViewer/QGLViewer/vec.cpp

   libQGLViewer//QGLViewer/VRender/NVector3.cpp
   libQGLViewer//QGLViewer/VRender/VisibilityOptimizer.cpp
   libQGLViewer//QGLViewer/VRender/Vector3.cpp
   libQGLViewer//QGLViewer/VRender/PrimitivePositioning.cpp
   libQGLViewer//QGLViewer/VRender/Vector2.cpp
   libQGLViewer//QGLViewer/VRender/Exporter.cpp
   libQGLViewer//QGLViewer/VRender/BSPSortMethod.cpp
   libQGLViewer//QGLViewer/VRender/VRender.cpp
   libQGLViewer//QGLViewer/VRender/FIGExporter.cpp
   libQGLViewer//QGLViewer/VRender/BackFaceCullingOptimizer.cpp
   libQGLViewer//QGLViewer/VRender/TopologicalSortMethod.cpp
   libQGLViewer//QGLViewer/VRender/EPSExporter.cpp
   libQGLViewer//QGLViewer/VRender/Primitive.cpp
   libQGLViewer//QGLViewer/VRender/gpc.cpp
   libQGLViewer//QGLViewer/VRender/ParserGL.cpp
)

set( UI_FILES
   libQGLViewer//QGLViewer/VRenderInterface.ui
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

add_library(${LIB} STATIC ${SOURCES})

target_link_libraries(${LIB}
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::OpenGL
   Qt5::Xml
)
