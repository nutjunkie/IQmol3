find_package(Qt5 COMPONENTS Core REQUIRED)
set(LIB Fmm)

set(HEADERS
)

set( SOURCES
)

set( UI_FILES
)

qt5_wrap_ui(UI_HEADERS ${UI_FILES} )

qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} ${SOURCES} ${UI_HEADERS})
target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
)
