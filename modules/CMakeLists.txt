find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL Xml REQUIRED)
set(LIB QGLViewer)

set(SOURCES
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
