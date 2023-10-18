set(LIB Gromacs)
find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

set( HEADERS
   GromacsDialog.h
   GromacsServerDialog.h
   GromacsConfigDialog.h

)

set( SOURCES
   GromacsDialog.C
   GromacsServerDialog.C
   GromacsConfigDialog.C
)

set( UI_FILES
   GromacsDialog.ui
   GromacsServerDialog.ui
   GromacsConfigDialog.ui
)

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} ${SOURCES} ${UI_HEADERS})
target_include_directories(${LIB} PUBLIC
   "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(${LIB} PUBLIC
   Network
   Process
   Util
)
target_link_libraries(${LIB} PRIVATE
   Data
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Network
)

