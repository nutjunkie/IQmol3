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

set( UI_FILES
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../Util)

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
add_library(${LIB} STATIC ${SOURCES} ${UI_FILES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui
   Qt5::Widgets
)
