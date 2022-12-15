find_package(Qt5 COMPONENTS Core Xml OpenGL REQUIRED)
set(LIB Data)

set( HEADERS
)

set( SOURCES
   Atom.C
   AtomicProperty.C
   AtomicDensity.C
   Bank.C
   CanonicalOrbitals.C
   ChargeMultiplicity.C
   Constraint.C
   Data.C
   DataFactory.C
   Density.C
   EfpFragment.C
   EfpFragmentLibrary.C
   ElectronicTransition.C
   Energy.C
   ExcitedStates.C
   File.C
   Frequencies.C
   GeminalOrbitals.C
   Geometry.C
   GeometryList.C
   GridData.C
   GridSize.C
   Hessian.C
   Mesh.C
   MultipoleExpansion.C
   NaturalBondOrbitals.C
   NaturalTransitionOrbitals.C
   NmrData.C
   NmrReference.C
   NmrReferenceLibrary.C
   OrbitalSymmetries.C
   Orbitals.C
   OrbitalFactory.C
   OrbitalsList.C
   PointCharge.C
   PointGroup.C
   PovRay.C
   RemSectionData.C
   Shell.C
   ShellList.C
   Surface.C
   SurfaceInfo.C
   SurfaceType.C
   VibrationalMode.C
   YamlNode.C
)

qt5_wrap_cpp( SOURCES ${HEADERS} )

add_library(${LIB} ${SOURCES})
target_include_directories(${LIB} PUBLIC
   "${${LIB}_SOURCE_DIR}"
)
target_include_directories(${LIB} PRIVATE
   # This is only because the OpenMeshCore target doesn't properly set the
   # interface headers
   "${OPENMESH_INCLUDE_DIR}"
   "${OpenBabel3_INCLUDE_DIRS}"
)
target_link_libraries(${LIB} PRIVATE
   Math
   ${OpenBabel3_LIBRARIES}
   OpenMeshCore
   Qt5::Core
   Qt5::Xml
   Qt5::OpenGL
)
