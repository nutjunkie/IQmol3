find_package(Qt5 COMPONENTS Core Gui Widgets Network Xml OpenGL REQUIRED)
set(LIB Parser)

set( SOURCES
   CartesianCoordinatesParser.C
   CubeParser.C
   EfpFragmentParser.C
   ExternalChargesParser.C
   FormattedCheckpointParser.C
   GdmaParser.C
   IQmolParser.C
   MeshParser.C
   OpenBabelParser.C
   ParseFile.C
   Parser.C
   PovRayParser.C
   QChemInputParser.C
   QChemOutputParser.C
   QChemPlotParser.C
   XyzParser.C
   YamlParser.C
   ZMatrixCoordinatesParser.C
)

if (QARCHIVE)
   list(APPEND SOURCES ArchiveParser.C)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

add_library(${LIB} STATIC ${SOURCES} ${UI_FILES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui
   Qt5::Widgets
   Qt5::Network
   Qt5::Xml
   Qt5::OpenGL
)
