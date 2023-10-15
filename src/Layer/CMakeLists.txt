set(LIB Layer)

find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)

set( HEADERS
   AtomLayer.h
   AxesLayer.h
   AxesMeshLayer.h
   BackgroundLayer.h
   BondLayer.h
   CanonicalOrbitalsLayer.h
   ChargeLayer.h
   ClippingPlaneLayer.h
   ComponentLayer.h
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
   MacroMoleculeLayer.h
   MolecularSurfacesLayer.h
   MoleculeLayer.h
   NaturalBondOrbitalsLayer.h
   NaturalTransitionOrbitalsLayer.h
   NmrLayer.h
   OctreeLayer.h
   OrbitalsLayer.h
   PrimitiveLayer.h
   ProteinChainLayer.h
   RemLayer.h
   SolventLayer.h
   SurfaceLayer.h
   SymmetryLayer.h
   SystemLayer.h
   VibronicLayer.h
)

set( SOURCES
   AtomLayer.C
   AxesLayer.C
   AxesMeshLayer.C
   BackgroundLayer.C
   BondLayer.C
   Cartoon.C
   ComponentLayer.C
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
   MacroMoleculeLayer.C
   MolecularSurfacesLayer.C
   MoleculeLayer.C
   NaturalBondOrbitalsLayer.C
   NaturalTransitionOrbitalsLayer.C
   NmrLayer.C
   OctreeLayer.C
   OrbitalsLayer.C
   PrimitiveLayer.C
   ProteinChainLayer.C
   RemLayer.C
   SolventLayer.C
   SurfaceLayer.C
   SymmetryLayer.C
   SystemLayer.C
   VibronicLayer.C
)

qt5_wrap_cpp( SOURCES ${HEADERS} )

add_library(${LIB} ${SOURCES})

target_link_libraries(${LIB} PUBLIC
   Configurator
   Util
)

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)
