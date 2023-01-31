cmake_minimum_required(VERSION 3.14.4)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(WIN32)
	# cmake -DBUILD_SHARED=ON -G "MinGW Makefiles" ../
	# -DYAML_STATIC_DEFINE
   set(BOOST_ROOT             "C:/boost")
   set(MINGW_ROOT             "C:/mingw64")
   set(HDF5_ROOT              "C:/Users/andre/Development/hdf5")
   set(CMAKE_C_COMPILER       "${MINGW_ROOT}/bin/x86_64-w64-mingw32-gcc.exe")
   set(CMAKE_CXX_COMPILER     "${MINGW_ROOT}/bin/x86_64-w64-mingw32-g++.exe")
   set(CMAKE_Fortran_COMPILER "${MINGW_ROOT}/bin/x86_64-w64-mingw32-gfortran.exe")
   set(CMAKE_PREFIX_PATH      "C:/Qt/Qt5.12.12/5.12.12/mingw73_64/lib/cmake/Qt5")
   set(OPENSSL_ROOT_DIR       "${CMAKE_CURRENT_SOURCE_DIR}/modules/openssl")
   set(ZLIB_INCLUDE_DIR       "${MINGW_ROOT}/x86_64-w64-mingw32/include/")
   set(ZLIB_LIBRARY           "${MINGW_ROOT}/x86_64-w64-mingw32/lib/libz.a")
   set(QT_UIC_EXECUTABLE      "C:/Qt/Qt5.12.12/5.12.12/mingw73_64/bin/uic.exe")

   enable_language("RC")
   set(WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/windows.rc)
endif()

set(targetName IQmol)
project(${targetName} VERSION 3.1.1 LANGUAGES CXX C Fortran)


include(FortranCInterface)
#FortranCInterface_VERIFY(CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)


set(NO_WARNING "-Wno-deprecated")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${NO_WARNING}")

set(CMAKE_MODULE_PATH ${IQmol_SOURCE_DIR}/cmake)


find_package(Qt5 COMPONENTS Core Gui Xml PrintSupport Widgets OpenGL REQUIRED)

find_package(ZLIB      REQUIRED)
if(ZLIB_FOUND)
   include_directories(${ZLIB_INCLUDE_DIRS})
endif()
find_package(Threads   REQUIRED)
find_package(OpenGL    REQUIRED)
find_package(OpenSSL   REQUIRED)

  
# No CMakeLists.txt for libQGLViewer
include_directories(modules/libQGLViewer)
add_subdirectory(modules  EXCLUDE_FROM_ALL)

include_directories(modules/libssh2/include)
add_subdirectory(modules/libssh2  EXCLUDE_FROM_ALL)

include_directories(modules/openmesh/src)
add_subdirectory(modules/openmesh  EXCLUDE_FROM_ALL)

include_directories(modules/yaml-cpp/include)
add_subdirectory(modules/yaml-cpp  EXCLUDE_FROM_ALL)

include_directories(modules/openbabel/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/modules/openbabel/include)
add_subdirectory(modules/openbabel EXCLUDE_FROM_ALL)

#set(HDF5_USE_STATIC_LIBRARIES 1)
#include_directories(modules/hdf5/src)
#add_subdirectory(modules/hdf5  EXCLUDE_FROM_ALL)
#include_directories(/opt/homebrew/Cellar/hdf5/1.12.2/include)

#include_directories(modules/HighFive/include)
#add_subdirectory(modules/HighFive  EXCLUDE_FROM_ALL)

enable_testing()

if(QARCHIVE)
   find_package(HDF5 REQUIRED)
   find_package(Armadillo REQUIRED)
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

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(src)
if (${Qt5Core_VERSION_STRING} VERSION_GREATER_EQUAL 5.15 )
   qt_add_resources(IQmol_RESOURCES_RCC resources/IQmol.qrc)
else ()
   qt5_add_resources(IQmol_RESOURCES_RCC resources/IQmol.qrc)
endif ()

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

if (LINUX)
   add_executable(${targetName} 
      src/main.C
      ${IQmol_RESOURCES_RCC}
   )
endif (LINUX)

if (WIN32)
   add_executable(${targetName} 
      src/main.C
      ${IQmol_RESOURCES_RCC}
      ${WIN32_RESOURCES}
   )
   set_target_properties(${targetName} PROPERTIES WIN32_EXECUTABLE ON)
endif (WIN32)

# The following is required to link gfortran statically, and was working before
# the compiler update.
#unset(CMAKE_Fortran_IMPLICIT_LINK_LIBRARIES)
#SET_TARGET_PROPERTIES(${targetName} PROPERTIES LINKER_LANGUAGE "CXX"
#  LINK_FLAGS
#  "/opt/homebrew/Cellar/gcc/11.3.0_2/lib/gcc/11/libgfortran.a 
#   /opt/homebrew/Cellar/gcc/11.3.0_2/lib/gcc/11/libquadmath.a  
#)

add_dependencies(Process Configurator)
add_dependencies(Viewer Process)

target_link_libraries (${targetName}
   Main
   Process
   Parser
   Network
   Qui 
   Layer
   Viewer
   Configurator
   Data
   Grid
   Util
   Math
   Plot
   Fort
   openbabel
   libssh2
   yaml-cpp
   QGLViewer
   Qt5::Core
   Qt5::Gui
   Qt5::Xml
   Qt5::PrintSupport
   Qt5::Widgets
   Qt5::OpenGL
   #OpenMeshCoreStatic
   #OpenMeshToolsStatic
   ${OPENMESH_LIBRARIES}
   #Boost::iostreams
   #Boost::serialization
   ${Boost_LIBRARIES}
   ${LIBSSH2_LIBRARY}
   ${OPENGL_LIBRARIES}
   ${ZLIB_LIBRARIES}
)

message("OPENGL_LIBRARIES " ${OPENGL_LIBRARIES})
message("ZLIB_LBRARIES "    ${ZLIB_LIBRARIES})
message("BOOST_LBRARIES "   ${Boost_LIBRARIES})
message("OPENMESH_LIBRARIES "   ${OPENMESH_LIBRARIES})

if(QARCHIVE)
   target_link_libraries (${targetName} archive_impl ${ARMADILLO_LIBRARY})
else(QARCHIVE)
#   target_link_libraries (${targetName} hdf5-static)
endif(QARCHIVE)

#if (APPLE)
#   target_link_libraries (${targetName} /opt/homebrew//lib/libzstd.a )
#endif(APPLE)
