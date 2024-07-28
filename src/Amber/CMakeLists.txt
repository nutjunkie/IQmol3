set(LIB Amber)
find_package(Qt5 COMPONENTS Core Gui Widgets Xml REQUIRED)

set( HEADERS
   ConfigDialog.h
   ParametrizeMoleculeDialog.h
   SystemBuilderDialog.h
)

set( SOURCES
   ConfigDialog.C
   ParametrizeMoleculeDialog.C
   SystemBuilderDialog.C
)

set( UI_FILES
   ConfigDialog.ui
   ParametrizeMoleculeDialog.ui
   SystemBuilderDialog.ui
)

qt5_wrap_ui(UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES} ${UI_HEADERS})

target_include_directories(${LIB} PUBLIC
   "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(${LIB} PUBLIC
   Configurator
   Util
)

target_link_libraries(${LIB} PRIVATE
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Xml
)
