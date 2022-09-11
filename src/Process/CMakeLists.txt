find_package(Qt5 COMPONENTS Core Gui Widgets Network Xml OpenGL REQUIRED)
set(LIB Process)

set(SOURCES
   Job.C
   JobInfo.C
   JobMonitor.C
   QChemJobInfo.C
   QueueOptionsDialog.C
   QueueResources.C
   QueueResourcesDialog.C
   QueueResourcesList.C
   Server.C
   ServerConfiguration.C
   ServerConfigurationDialog.C
   ServerConfigurationListDialog.C
   ServerRegistry.C
   SshFileDialog.C
   SystemDependent.C
)

if (WIN32)
   LIST(APPEND SOURCES getpids.C)
endif()

set( UI_FILES
   JobMonitor.ui
   QueueOptionsDialog.ui
   QueueResourcesDialog.ui
   ServerConfigurationDialog.ui
   ServerConfigurationListDialog.ui
   SshFileDialog.ui
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.. ../Util ../Yaml ../Data ../Network ../Parser ../Layer ../Configurator)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../Configurator)

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES} ${UI_FILES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::Network
   Qt5::OpenGL
)
