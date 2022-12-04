find_package(yaml-cpp)
if(NOT ${yaml-cpp_FOUND})
   FetchContent_Declare(
      yaml-cpp
      GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
      GIT_TAG yaml-cpp-0.7.0
   )
   set(YAML_CPP_BUILD_TESTS OFF)
   FetchContent_MakeAvailable(yaml-cpp)
endif(NOT ${yaml-cpp_FOUND})
