find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
set(LIB Plot)

set( HEADERS
   CustomPlot.h
   qcustomplot.h
)

set( SOURCES
   CustomPlot.C
   qcustomplot.cpp
)

qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} ${SOURCES})
target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}")
target_link_libraries(${LIB} PRIVATE
   Util
   Qt5::Core 
   Qt5::Gui
   Qt5::Widgets
)
