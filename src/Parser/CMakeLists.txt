find_package(Qt5 COMPONENTS Core Gui Widgets Network Xml OpenGL REQUIRED)
set(LIB Parser)

set( HEADERS
#  ArchiveParser.h
#  CartesianCoordinatesParser.h
#  CubeParser.h
#  EfpFragmentParser.h
#  ExternalChargesParser.h
#  FormattedCheckpointParser.h
#  GdmaParser.h
#  IQmolParser.h
#  MeshParser.h
#  OpenBabelParser.h
   ParseFile.h
#  Parser.h
#  PovRayParser.h
#  QChemInputParser.h
#  QChemOutputParser.h
#  QChemPlotParser.h
#  ReorderBasis.h
#  TextStream.h
#  XyzParser.h
#  YamlParser.h
#  ZMatrixCoordinatesParser.h
)

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
   ReorderBasis.C
   XyzParser.C
   YamlParser.C
   ZMatrixCoordinatesParser.C
)

if (QARCHIVE)
   list(APPEND SOURCES ArchiveParser.C)
endif()

qt5_wrap_cpp(SOURCES ${HEADERS} )

add_library(${LIB} ${SOURCES})
target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}")
target_include_directories(${LIB} PRIVATE
   "${OpenBabel3_INCLUDE_DIRS}"
)
target_link_libraries(${LIB} PRIVATE
   ${OpenBabel3_LIBRARIES}
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Network
   Qt5::Xml
   Qt5::OpenGL
)
