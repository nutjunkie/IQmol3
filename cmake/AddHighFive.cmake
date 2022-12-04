find_package(HighFive 2.3.1)
if(NOT ${HighFive_FOUND})
   FetchContent_Declare(
      HighFive
      GIT_REPOSITORY https://github.com/BlueBrain/HighFive.git
      GIT_TAG cfc19a1538b106d04a2d3e5806383f1580975d23
   )
   #FetchContent_MakeAvailable(HighFive)
   # include_directories(modules/HighFive/include)
endif(NOT ${HighFive_FOUND})
