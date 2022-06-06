set(LIB Layer)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt5 COMPONENTS Xml OpenGL REQUIRED)

set( HEADERS
   AtomLayer.h
   AxesLayer.h
   AxesMeshLayer.h
   BackgroundLayer.h
   BondLayer.h
   CanonicalOrbitalsLayer.h
   ChargeLayer.h
   ClippingPlaneLayer.h
   ConstraintLayer.h
   ContainerLayer.h
   CubeDataLayer.h
   DipoleLayer.h
   DysonOrbitalsLayer.h
   EfpFragmentLayer.h
   EfpFragmentListLayer.h
   ExcitedStatesLayer.h
   FileLayer.h
   FrequenciesLayer.h
   GLObjectLayer.h
   GeminalOrbitalsLayer.h
   GeometryLayer.h
   GeometryListLayer.h
   GlobalLayer.h
   GroupLayer.h
   InfoLayer.h
   IsotopesLayer.h
   Layer.h
   LayerFactory.h
   MolecularSurfacesLayer.h
   MoleculeLayer.h
   NaturalBondOrbitalsLayer.h
   NaturalTransitionOrbitalsLayer.h
   NmrLayer.h
   OrbitalsLayer.h
   PrimitiveLayer.h
   RemLayer.h
   SurfaceLayer.h
   SymmetryLayer.h
)

set( SOURCES
   AtomLayer.C
   AxesLayer.C
   AxesMeshLayer.C
   BackgroundLayer.C
   BondLayer.C
   CanonicalOrbitalsLayer.C
   ChargeLayer.C
   ClippingPlaneLayer.C
   ConstraintLayer.C
   ContainerLayer.C
   CubeDataLayer.C
   DipoleLayer.C
   DysonOrbitalsLayer.C
   EfpFragmentLayer.C
   EfpFragmentListLayer.C
   ExcitedStatesLayer.C
   FileLayer.C
   FrequenciesLayer.C
   GeminalOrbitalsLayer.C
   GeometryLayer.C
   GeometryListLayer.C
   GroupLayer.C
   InfoLayer.C
   IsotopesLayer.C
   Layer.C
   LayerFactory.C
   MolecularSurfacesLayer.C
   MoleculeLayer.C
   NaturalBondOrbitalsLayer.C
   NaturalTransitionOrbitalsLayer.C
   NmrLayer.C
   OrbitalsLayer.C
   PrimitiveLayer.C
   RemLayer.C
   SurfaceLayer.C
   SymmetryLayer.C
)

include_directories(../ ../Data ../Util ../Viewer ../Old ../Grid ../Parser ../Process ../Configurator)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Configurator)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Grid)

qt5_wrap_cpp( SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES} ${UI_FILES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)
