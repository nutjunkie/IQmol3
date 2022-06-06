cmake_minimum_required(VERSION 3.20.2)
set(targetName IQmol3)

project(${targetName} VERSION 3.0.0 LANGUAGES CXX Fortran)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

unset(CMAKE_Fortran_IMPLICIT_LINK_LIBRARIES)

if(APPLE)
   set(CMAKE_PREFIX_PATH /opt/homebrew//Cellar/qt@5/5.15.3/lib/cmake/Qt5)
endif(APPLE)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
 
set(Boost_USE_STATIC_LIBS TRUE)
find_package(Boost COMPONENTS iostreams serialization REQUIRED)

# This does not appear to be working, need cmake -DBUILD_SHARED=OFF ../
add_definitions(-DGL_SILENCE_DEPRECATION -DBUILD_SHARED=OFF)

add_subdirectory(modules/openmesh EXCLUDE_FROM_ALL)
add_subdirectory(modules/openbabel EXCLUDE_FROM_ALL)
add_subdirectory(modules/yaml-cpp EXCLUDE_FROM_ALL)
add_subdirectory(modules EXCLUDE_FROM_ALL)  # libQGLViewer

include_directories(${Boost_INCLUDE_DIRS})
include_directories(modules/yaml-cpp/include)
include_directories(modules/openmesh/src)
include_directories(modules/libQGLViewer)
include_directories(modules/openbabel/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/modules/openbabel/include)

add_subdirectory(src)
qt_add_resources(IQmol_RESOURCES_RCC src/Main/IQmol.qrc)

add_executable(${targetName} 
   src/main.C
   ${IQmol_RESOURCES_RCC}
)

target_link_libraries (${targetName}
   Configurator
   Data
   Grid
   Layer
   Main
   Network
   Old 
   Parser
   Plot
   Process
   Qui 
   Util
   Viewer
  openbabel
  yaml-cpp
  OpenMeshCoreStatic
  OpenMeshToolsStatic
  QGLViewer
  ${Boost_LIBRARIES}
)

