find_package(Qt5 COMPONENTS Core Xml OpenGL REQUIRED)
set(LIB Data)

set( HEADERS
   Residue.h
   Group.h
)

set( SOURCES
   AminoAcid.C
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
   PdbData.C
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
   Vibronic.C
   YamlNode.C
)

qt5_wrap_cpp( SOURCES ${HEADERS} )

add_library(${LIB} STATIC ${SOURCES})
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
#   Math
#   ${OpenBabel3_LIBRARIES}
#   ${OPENMESH_LIBRARIES}
   Qt5::Gui
   Qt5::Core
   Qt5::Xml
   Qt5::OpenGL  # required for Vec, go figure
#   yaml-cpp
#   Util
#   ${Boost_LIBRARIES}
)
