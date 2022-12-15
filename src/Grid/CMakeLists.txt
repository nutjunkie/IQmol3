find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)
set(LIB Grid)

set(HEADERS
   BasisEvaluator.h
   BoundingBoxDialog.h
   DensityEvaluator.h
   GridEvaluator.h
   GridEvaluator.h
   GridInfoDialog.h
   GridProduct.h
   MarchingCubes.h
   MeshDecimator.h
   MolecularGridEvaluator.h
   OrbitalEvaluator.h
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
   SpatialProperty.C
   Spline.C
   SurfaceGenerator.C
)

set( UI_FILES
   BoundingBoxDialog.ui
   GridInfoDialog.ui
)

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )

add_library(${LIB} ${SOURCES} ${UI_HEADERS})

target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(${LIB} PRIVATE
   Data
   Util
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)
