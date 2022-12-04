find_package(OpenMesh)
if(NOT ${OPENMESH_FOUND})
   FetchContent_Declare(
      openmesh
      GIT_REPOSITORY https://github.com/Lawrencemm/openmesh.git
      GIT_TAG 4e2e481f438747d64e62899021da9f469fd9daf8
   )
   FetchContent_MakeAvailable(openmesh)
endif(NOT ${OPENMESH_FOUND})
# #include_directories(${OPENMESH_INCLUDE_DIR})
# include_directories(modules/openmesh/src)
