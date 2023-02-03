set(LIB OSGViewer)

find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)

set( HEADERS
   OSGViewer.h
)

set( SOURCES
   OSGViewer.C
)

set( UI_FILES
)

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} ${SOURCES} ${UI_HEADERS})

target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(${LIB} PRIVATE
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::OpenGL
)
