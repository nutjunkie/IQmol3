find_package(HDF5 1.12.2)
if(NOT ${HDF5_FOUND})
   FetchContent_Declare(
      hdf5
      GIT_REPOSITORY https://github.com/HDFGroup/hdf5.git
      GIT_TAG hdf5-1_12_2
   )
   FetchContent_MakeAvailable(hdf5)
endif(NOT ${HDF5_FOUND})
