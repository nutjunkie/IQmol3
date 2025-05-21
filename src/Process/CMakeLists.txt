find_package(Qt5 COMPONENTS Core Gui Widgets Network Xml OpenGL REQUIRED)
set(LIB Process)

set( HEADERS 
   AwsConfigurationDialog.h
   Job.h
   JobMonitor.h
   QueueOptionsDialog.h
   QueueResourcesDialog.h
   Server.h
   ServerConfigurationDialog.h
   ServerConfigurationListDialog.h
   SshFileDialog.h
)

set( SOURCES
   AwsConfigurationDialog.C
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
   list(APPEND SOURCES getpids.C)
endif()

set( UI_FILES
   AwsConfigurationDialog.ui
   JobMonitor.ui
   QueueOptionsDialog.ui
   QueueResourcesDialog.ui
   ServerConfigurationDialog.ui
   ServerConfigurationListDialog.ui
   SshFileDialog.ui
)

qt5_wrap_ui( UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES} ${UI_HEADERS})
target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${LIB} PUBLIC
   Network
)
target_link_libraries(${LIB} PRIVATE
   Data
   Layer # FileLayer for JobMonitor::viewOutput
   Parser
   Util
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Xml
   Qt5::Network
   Qt5::OpenGL
)

