set(LIB Configurator)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt5 COMPONENTS Xml OpenGL REQUIRED)

set( HEADERS
   AxesConfigurator.h
   AxesMeshConfigurator.h
   BackgroundConfigurator.h
   ClippingPlaneConfigurator.h
   Configurator.h
   ConstraintConfigurator.h
   CubeDataConfigurator.h
   DipoleConfigurator.h
   EfpFragmentListConfigurator.h
   ExcitedStatesConfigurator.h
   FileConfigurator.h
   FrequenciesConfigurator.h
   GeminalOrbitalsConfigurator.h
   GeometryListConfigurator.h
   InfoConfigurator.h
   IsotopesConfigurator.h
   MolecularSurfacesConfigurator.h
   MoleculeConfigurator.h
   MullikenDecompositionsDialog.h
   NmrConfigurator.h
   OrbitalsConfigurator.h
   SurfaceAnimatorDialog.h
   SurfaceConfigurator.h
   SymmetryConfigurator.h
)

set( SOURCES
   AxesConfigurator.C
   AxesMeshConfigurator.C
   BackgroundConfigurator.C
   ClippingPlaneConfigurator.C
   ConstraintConfigurator.C
   CubeDataConfigurator.C
   DipoleConfigurator.C
   EfpFragmentListConfigurator.C
   ExcitedStatesConfigurator.C
   FileConfigurator.C
   FrequenciesConfigurator.C
   GeminalOrbitalsConfigurator.C
   GeometryListConfigurator.C
   InfoConfigurator.C
   IsotopesConfigurator.C
   MolecularSurfacesConfigurator.C
   MoleculeConfigurator.C
   MullikenDecompositionsDialog.C
   NmrConfigurator.C
   OrbitalsConfigurator.C
   SurfaceAnimatorDialog.C
   SurfaceConfigurator.C
   SymmetryConfigurator.C
)

set( UI_FILES
   AxesConfigurator.ui
   AxesMeshConfigurator.ui
   BackgroundConfigurator.ui
   ClippingPlaneConfigurator.ui
   CubeDataConfigurator.ui
   DipoleConfigurator.ui
   EfpFragmentListConfigurator.ui
   ExcitedStatesConfigurator.ui
   FileConfigurator.ui
   FrequenciesConfigurator.ui
   GeminalOrbitalsConfigurator.ui
   GeometryListConfigurator.ui
   InfoConfigurator.ui
   IsotopesConfigurator.ui
   MolecularSurfacesConfigurator.ui
   MoleculeConfigurator.ui
   MullikenDecompositionsDialog.ui
   NmrConfigurator.ui
   OrbitalsConfigurator.ui
   ScalarConstraintConfigurator.ui
   SurfaceAnimatorDialog.ui
   SurfaceConfigurator.ui
   SymmetryConfigurator.ui
   VectorConstraintConfigurator.ui
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../ ../Data ../Util ../Layer ../Old ../Viewer ../Plot ../Grid)

qt5_wrap_ui(SOURCES ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)
