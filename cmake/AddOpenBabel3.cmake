find_package(OpenBabel3)
if(NOT ${OpenBabel3_FOUND})
   FetchContent_Declare(
      openbabel
      GIT_REPOSITORY https://github.com/openbabel/openbabel.git
      GIT_TAG 530dbfa333189f4f429bd01a411b866ff5e2efe9
   )
   FetchContent_MakeAvailable(openbabel)
endif(NOT ${OpenBabel3_FOUND})
