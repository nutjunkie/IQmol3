find_package(LibSsh2)
if(NOT ${LIBSSH2_FOUND})
   FetchContent_Declare(
      libssh2
      GIT_REPOSITORY https://github.com/libssh2/libssh2.git
      GIT_TAG 821d50dad313b53fb2782f26aec1f52f1be34fc0
   )
   FetchContent_MakeAvailable(libssh2)
endif(NOT ${LIBSSH2_FOUND})
