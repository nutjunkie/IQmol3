set(LIB Configurator)

find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)

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
   GenerateConformersDialog.h
   GeometryListConfigurator.h
   InfoConfigurator.h
   IsotopesConfigurator.h
   MolecularSurfacesConfigurator.h
   MoleculeConfigurator.h
   MullikenDecompositionsDialog.h
   NmrConfigurator.h
   OctreeConfigurator.h
   OrbitalsConfigurator.h
   SurfaceAnimatorDialog.h
   SurfaceConfigurator.h
   SymmetryConfigurator.h
   VibronicConfigurator.h
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
   GenerateConformersDialog.C
   GeometryListConfigurator.C
   InfoConfigurator.C
   IsotopesConfigurator.C
   MolecularSurfacesConfigurator.C
   MoleculeConfigurator.C
   MullikenDecompositionsDialog.C
   NmrConfigurator.C
   OctreeConfigurator.C
   OrbitalsConfigurator.C
   SurfaceAnimatorDialog.C
   SurfaceConfigurator.C
   SymmetryConfigurator.C
   VibronicConfigurator.C
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
   GenerateConformersDialog.ui
   GeometryListConfigurator.ui
   InfoConfigurator.ui
   IsotopesConfigurator.ui
   MolecularSurfacesConfigurator.ui
   MoleculeConfigurator.ui
   MullikenDecompositionsDialog.ui
   NmrConfigurator.ui
   OctreeConfigurator.ui
   OrbitalsConfigurator.ui
   ScalarConstraintConfigurator.ui
   SurfaceAnimatorDialog.ui
   SurfaceConfigurator.ui
   SymmetryConfigurator.ui
   VectorConstraintConfigurator.ui
   VibronicConfigurator.ui
)

qt5_wrap_ui(UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} ${SOURCES} ${UI_HEADERS})

target_include_directories(${LIB} PUBLIC
   "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
   "${OpenBabel3_INCLUDE_DIRS}"
)

target_link_libraries(${LIB} PUBLIC
   Data
   #Layer
   Util
   ${OpenBabel3_LIBRARIES}
)

target_link_libraries(${LIB} PRIVATE
#  Grid
   Plot
   # Process # QChemJobInfo
   # Viewer
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)
