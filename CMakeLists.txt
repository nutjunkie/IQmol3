cmake_minimum_required(VERSION 3.20.2)
set(targetName IQmol)

project(${targetName} VERSION 3.0.0 LANGUAGES CXX Fortran)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

if(APPLE)
   set(CMAKE_PREFIX_PATH /opt/homebrew//Cellar/qt@5/5.15.3/lib/cmake/Qt5)
endif(APPLE)

#set(CMAKE_MODULE_PATH ${IQmol_SOURCE_DIR}/cmake)
#find_package(LibSSH2 REQUIRED)
#message("LIBSSH2_FOUND =       " ${LIBSSH2_FOUND})
#message("LIBSSH2_INCLUDE_DIR = " ${LIBSSH2_INCLUDE_DIR})
#message("LIBSSH2_LIBRARY=      " ${LIBSSH2_LIBRARY})

find_package(Qt5 COMPONENTS Core Gui Xml PrintSupport Widgets OpenGL REQUIRED)

find_package(ZLIB    REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL  REQUIRED)
  
# This does not appear to be working, need cmake -DBUILD_SHARED=OFF ../
add_definitions(-DGL_SILENCE_DEPRECATION -DBUILD_SHARED=OFF)

# External modules
include_directories(modules/libQGLViewer)
include_directories(modules/libssh2/include)
include_directories(modules/openmesh/src)
include_directories(modules/yaml-cpp/include)
include_directories(modules/openbabel/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/modules/openbabel/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

add_subdirectory(modules/libssh2   EXCLUDE_FROM_ALL)
add_subdirectory(modules/openmesh  EXCLUDE_FROM_ALL)
add_subdirectory(modules/openbabel EXCLUDE_FROM_ALL)
add_subdirectory(modules/yaml-cpp  EXCLUDE_FROM_ALL)
add_subdirectory(modules           EXCLUDE_FROM_ALL)  # no CMakeLists.txt for libQGLViewer

# The following must come after the modules as it picks up the include 
# for the system libssh2.h.
set(Boost_USE_STATIC_LIBS TRUE)
find_package(Boost COMPONENTS iostreams serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


add_subdirectory(src)
qt_add_resources(IQmol_RESOURCES_RCC resources/IQmol.qrc)

set(ICON_PATH resources/IQmol.icns)

if (APPLE)
   add_executable(${targetName} 
      MACOSX_BUNDLE
      ${ICON_PATH}
      src/main.C
      ${IQmol_RESOURCES_RCC}
   )
   set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
   file(COPY ${ICON_PATH} DESTINATION "bin/${targetName}.app/Contents/Resources")
   file(COPY resources/crtimgseq.py DESTINATION "bin/${targetName}.app/Contents/Resources")
   add_custom_command(TARGET ${targetName} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
      copy_directory ${CMAKE_SOURCE_DIR}/share ${CMAKE_BINARY_DIR}/bin/${targetName}.app/Contents/share/)
   add_custom_command(TARGET ${targetName} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
      copy_directory ${CMAKE_SOURCE_DIR}/modules/openbabel/data 
                     ${CMAKE_BINARY_DIR}/bin/${targetName}.app/Contents/share/openbabel
   )
endif(APPLE)

unset(CMAKE_Fortran_IMPLICIT_LINK_LIBRARIES)
set_target_properties(${targetName} PROPERTIES LINKER_LANGUAGE "CXX"
  LINK_FLAGS "/opt/homebrew//Cellar/gcc/11.3.0/lib/gcc/11/libgfortran.a -lgcc"
#    libquadmath.a -lm -lgcc"
#  "/usr/local/Cellar/gcc/7.1.0/lib/gcc/7/
#  /usr/local/Cellar/gcc/7.1.0/lib/gcc/7/
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
   Math
   Viewer
   openbabel
   libssh2
   yaml-cpp
   OpenMeshCoreStatic
   OpenMeshToolsStatic
   QGLViewer
   Qt5::Core
   Qt5::Gui
   Qt5::Xml
   Qt5::PrintSupport
   Qt5::Widgets
   Qt5::OpenGL
   ${Boost_LIBRARIES}
   ${LIBSSH2_LIBRARY}
   ${OPENGL_LIBRARIES}
)
