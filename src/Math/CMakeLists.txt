find_package(Qt5 COMPONENTS OpenGL Xml REQUIRED)
set(LIB Math)

set( SOURCES
   Align.C
   EulerAngles.C
   Function.C
   Matrix.C
   qcprot.C
   Spline.C
)

add_library(${LIB} ${SOURCES})
target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}")
target_link_libraries(${LIB} PRIVATE
   QGLViewer
   Qt5::Xml
   Qt5::OpenGL
)
