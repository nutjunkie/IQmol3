cmake_minimum_required(VERSION 3.4)
project(IQmol3 CXX)

set(targetName IQmol3)


set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})
set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DBUILD_SHARED=OFF)

add_subdirectory(modules/openmesh EXCLUDE_FROM_ALL)
add_subdirectory(modules/openbabel EXCLUDE_FROM_ALL)
add_subdirectory(modules/yaml-cpp EXCLUDE_FROM_ALL)

#target_compile_definitions(openbabel PUBLIC BUILD_SHARED=OFF)


set(SOURCES src/main.C)

add_executable(${targetName} ${SOURCES})
target_link_libraries (${targetName}
  openbabel
  yaml-cpp
  OpenMeshCoreStatic
  OpenMeshToolsStatic
)

