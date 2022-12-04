# find_package(QGLViewer)
# if(NOT ${QGLVIEWER_FOUND})
    FetchContent_Declare(
        libQGLViewer
        GIT_REPOSITORY https://github.com/GillesDebunne/libQGLViewer.git
        GIT_TAG 45a4e92aa60f3c33eed948b6f421e696a2a171f4
    )
    FetchContent_GetProperties(libQGLViewer)
    if(NOT libQGLViewer_POPULATED)
    endif()
    set( HEADERS
        libQGLViewer/QGLViewer/VRender/Primitive.h
        libQGLViewer/QGLViewer/VRender/Vector2.h
        libQGLViewer/QGLViewer/VRender/Types.h
        libQGLViewer/QGLViewer/VRender/AxisAlignedBox.h
        libQGLViewer/QGLViewer/VRender/PrimitivePositioning.h
        libQGLViewer/QGLViewer/VRender/VRender.h
        libQGLViewer/QGLViewer/VRender/SortMethod.h
        libQGLViewer/QGLViewer/VRender/Optimizer.h
        libQGLViewer/QGLViewer/VRender/gpc.h
        libQGLViewer/QGLViewer/VRender/NVector3.h
        libQGLViewer/QGLViewer/VRender/ParserGL.h
        libQGLViewer/QGLViewer/VRender/Exporter.h
        libQGLViewer/QGLViewer/VRender/Vector3.h
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

        libQGLViewer/QGLViewer/VRender/NVector3.cpp
        libQGLViewer/QGLViewer/VRender/VisibilityOptimizer.cpp
        libQGLViewer/QGLViewer/VRender/Vector3.cpp
        libQGLViewer/QGLViewer/VRender/PrimitivePositioning.cpp
        libQGLViewer/QGLViewer/VRender/Vector2.cpp
        libQGLViewer/QGLViewer/VRender/Exporter.cpp
        libQGLViewer/QGLViewer/VRender/BSPSortMethod.cpp
        libQGLViewer/QGLViewer/VRender/VRender.cpp
        libQGLViewer/QGLViewer/VRender/FIGExporter.cpp
        libQGLViewer/QGLViewer/VRender/BackFaceCullingOptimizer.cpp
        libQGLViewer/QGLViewer/VRender/TopologicalSortMethod.cpp
        libQGLViewer/QGLViewer/VRender/EPSExporter.cpp
        libQGLViewer/QGLViewer/VRender/Primitive.cpp
        libQGLViewer/QGLViewer/VRender/gpc.cpp
        libQGLViewer/QGLViewer/VRender/ParserGL.cpp
    )

    set( UI_FILES
        libQGLViewer/QGLViewer/VRenderInterface.ui
    )

    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)

    add_library(QGLViewer ${SOURCES})
    set_target_properties(QGLViewer
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")

    find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL Xml REQUIRED)

    target_link_libraries(QGLViewer
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::OpenGL
        Qt5::Xml
    )
# endif(NOT ${QGLVIEWER_FOUND})
