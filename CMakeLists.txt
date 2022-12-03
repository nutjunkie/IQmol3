cmake_minimum_required(VERSION 3.14.4)

set(targetName IQmol)
project(${targetName} VERSION 3.0.0 LANGUAGES CXX C Fortran)

include(FortranCInterface)
#FortranCInterface_VERIFY(CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

#if(APPLE)
#   set(CMAKE_PREFIX_PATH /opt/homebrew//Cellar/qt@5/5.15.6/lib/cmake/Qt5)
#endif(APPLE)

set(CMAKE_MODULE_PATH ${IQmol_SOURCE_DIR}/cmake)
#find_package(HDF5 REQUIRED)
#find_package(LibSSH2 REQUIRED)

#message("LIBSSH2_FOUND =       " ${LIBSSH2_FOUND})
#message("LIBSSH2_INCLUDE_DIR = " ${LIBSSH2_INCLUDE_DIR})
#message("LIBSSH2_LIBRARY=      " ${LIBSSH2_LIBRARY})

find_package(Qt5 COMPONENTS Core Gui Xml PrintSupport Widgets OpenGL REQUIRED)

find_package(ZLIB      REQUIRED)
find_package(Threads   REQUIRED)
find_package(OpenGL    REQUIRED)
#find_package(Armadillo REQUIRED)
  
# No CMakeLists.txt for libQGLViewer
include_directories(modules/libQGLViewer)
add_subdirectory(modules  EXCLUDE_FROM_ALL)

include_directories(modules/libssh2/include)
add_subdirectory(modules/libssh2  EXCLUDE_FROM_ALL)

include_directories(modules/openmesh/src)
add_subdirectory(modules/openmesh  EXCLUDE_FROM_ALL)

include_directories(modules/yaml-cpp/include)
add_subdirectory(modules/yaml-cpp  EXCLUDE_FROM_ALL)


# There seems to be a scoping problem with the following variables, so they need to be set
# on the command line in order for them to take effect.
# cmake -DBUILD_SHARED=OFF  ../ 
#set(BUILD_SHARED OFF)
#set(GL_SILENCE_DEPRECATION ON)
include_directories(modules/openbabel/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/modules/openbabel/include)
add_subdirectory(modules/openbabel EXCLUDE_FROM_ALL)

set(HDF5_USE_STATIC_LIBRARIES 1)
#include_directories(modules/hdf5/src)
#add_subdirectory(modules/hdf5  EXCLUDE_FROM_ALL)
include_directories(/opt/homebrew/Cellar/hdf5/1.12.2/include)

include_directories(modules/HighFive/include)
add_subdirectory(modules/HighFive  EXCLUDE_FROM_ALL)

enable_testing()

if(QARCHIVE)
   add_compile_definitions(WITH_AVIEW QARCHIVE)
   include_directories(qarchive/libctx)
   include_directories(qarchive/libaview)
   include_directories(qarchive/libstore)
   include_directories(qarchive/libarchive)
   include_directories(${CMAKE_CURRENT_BINARY_DIR}/qarchive/libarchive/autogen_impl)
   add_subdirectory(qarchive)
endif(QARCHIVE)

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
#   add_custom_command(TARGET ${targetName} POST_BUILD 
#      COMMAND macdeployqt ${CMAKE_BINARY_DIR}/bin/${targetName}.app)
endif(APPLE)

if (UNIX)
   add_executable(${targetName} 
      src/main.C
      ${IQmol_RESOURCES_RCC}
   )
endif (UNIX)

# The following is required to link gfortran statically, and was working before
# the compiler update.
#unset(CMAKE_Fortran_IMPLICIT_LINK_LIBRARIES)
#SET_TARGET_PROPERTIES(${targetName} PROPERTIES LINKER_LANGUAGE "CXX"
#  LINK_FLAGS
#  "/opt/homebrew/Cellar/gcc/11.3.0_2/lib/gcc/11/libgfortran.a 
#   /opt/homebrew/Cellar/gcc/11.3.0_2/lib/gcc/11/libquadmath.a  
#)

add_dependencies(Old Configurator)
add_dependencies(Process Configurator)
add_dependencies(Viewer Old Process)

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

if(QARCHIVE)
   target_link_libraries (${targetName} archive_impl ${ARMADILLO_LIBRARY})
else(QARCHIVE)
   target_link_libraries (${targetName} hdf5-static)
endif(QARCHIVE)
