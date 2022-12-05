find_package(ZLIB)
if(NOT ${ZLIB_FOUND})
   FetchContent_Declare(
      zlib
      GIT_REPOSITORY https://github.com/madler/zlib.git
      GIT_TAG v1.2.13
   )
   FetchContent_MakeAvailable(zlib)
endif(NOT ${ZLIB_FOUND})
