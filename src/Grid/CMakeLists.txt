find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)
set(LIB Grid)

set(HEADERS
   BasisEvaluator.h
   BoundingBoxDialog.h
   DensityEvaluator.h
   GridEvaluator.h
   GridInfoDialog.h
   GridProduct.h
   Lebedev.h
   MarchingCubes.h
   MarchingCubesData.h
   MeshDecimator.h
   MolecularGridEvaluator.h
   OrbitalEvaluator.h
   Spline.h
   SurfaceGenerator.h
)

set( SOURCES
   BasisEvaluator.C
   BoundingBoxDialog.C
   DensityEvaluator.C
   GridEvaluator.C
   GridInfoDialog.C
   GridProduct.C
   Lebedev.C
   MarchingCubes.C
   MeshDecimator.C
   MolecularGridEvaluator.C
   OrbitalEvaluator.C
   Spline.C
   SurfaceGenerator.C
)

set( UI_FILES
   BoundingBoxDialog.ui
   GridInfoDialog.ui
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(.. ../Util ../Data)
include_directories(../OpenMesh/src)

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES} ${UI_FILES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)
