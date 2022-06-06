find_package(Qt5 COMPONENTS OpenGL Xml REQUIRED)
set(LIB Math)

set( HEADERS
   Align.h
   Axes.h
   EulerAngles.h
   Function.h
   Matrix.h
   Numerical.h
   qcprot.h
)

set( SOURCES
   Align.C
   EulerAngles.C
   Function.C
   Matrix.C
   qcprot.C
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${LIB} STATIC ${SOURCES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Xml
   Qt5::OpenGL
)
