find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)
set(LIB Network)

if (APPLE)
   # brew puts things in a non-standard location
   set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif()
find_package(OpenSSL REQUIRED)

set(HEADERS
   Connection.h
   HttpConnection.h
   HttpReply.h
   LocalConnection.h
   LocalReply.h
   Network.h
   NetworkException.h
   Reply.h
   SshConnection.h
   SshReply.h
)

set( SOURCES
   Connection.C
   HttpConnection.C
   HttpReply.C
   LocalConnection.C
   LocalReply.C
   Network.C
   SshConnection.C
   SshReply.C
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../Util)
include_directories(${OPENSSL_INCLUDE_DIR})

qt5_wrap_cpp( SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES})

target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui
   Qt5::Widgets
   Qt5::Network
)
