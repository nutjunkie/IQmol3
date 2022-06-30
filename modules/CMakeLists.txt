#----------------------------------------------------------------
# libQGLViewer
#----------------------------------------------------------------

find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL Xml REQUIRED)

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

add_library(QGLViewer STATIC ${SOURCES})

target_link_libraries(QGLViewer
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::OpenGL
   Qt5::Xml
)


#----------------------------------------------------------------
# ffmpeg
#----------------------------------------------------------------

#set( proj ffmpeg )
#set( SHARED_FFMPEG )
#
#set(BASIC
#   --extra-ldflags=-L/usr/local/lib
#   --extra-cflags=-I/usr/local/include
#   --enable-gpl
#   --enable-libx264
#)
#
#option( FFMPEG_GPL "Use a GPL version of FFMPEG" OFF )
#
#set(FFMPEG_SOURCE_DIR  ${CMAKE_SOURCE_DIR}/modules/ffmeg )
#set(FFMPEG_INSTALL_DIR ${CMAKE_BINARY_DIR}/modules/ffmeg )
#
#ExternalProject_Add(${proj}
#   SOURCE_DIR ${FFMPEG_SOURCE_DIR}
#   INSTALL_DIR ${FFMPEG_INSTALL_DIR}
#   # Build the project
#   BUILD_IN_SOURCE 0
#
#   # Configure step
#   # DO STH FOR THE ARCHITECTURE...
#   CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${FFMPEG_INSTALL_DIR} ${SHARED_FFMPEG} ${F#FMPEG_GPL_FLAG} ${BASIC}
#
#        #  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -j${NProcessors}
#)

# define the library suffix
#if( SUPER_SHARED_LIBS )
#    if( APPLE )
#        set(LIBRARY_SUFFIX .dylib)
#    else( APPLE )
#        set(LIBRARY_SUFFIX .so)
#    endif( APPLE )
#else( SUPER_SHARED_LIBS )
#    set(LIBRARY_SUFFIX .a)
#endif( SUPER_SHARED_LIBS )
#
## ADD DIRECTORIES FOR DEPENDENCIES IN Main/CMakeLists.txt
#set( FFMPEG_LIBRARYS libavcodec.a
#        libavformat.a
#        libavutil.a
#        libswscale.a
#        )
#
#include_directories(${FFMPEG_INSTALL_DIR}/include)
#
#set(FFMPEG_LIBRARY_DIRS ${FFMPEG_INSTALL_DIR}/lib/)
#link_directories(${FFMPEG_LIBRARY_DIRS})
