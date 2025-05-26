find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)
set(LIB Network)

set(HEADERS
   AwsConnection.h
   AwsReply.h
   Connection.h
   HttpConnection.h
   HttpReply.h
   LocalConnection.h
   LocalReply.h
   Reply.h
   SshConnection.h
   SshReply.h
)

set( SOURCES
   AwsConnection.C
   AwsReply.C
   Connection.C
   HttpConnection.C
   HttpReply.C
   LocalConnection.C
   LocalReply.C
   Network.C
   SshConnection.C
   SshReply.C
)

qt5_wrap_cpp( SOURCES ${HEADERS} )

add_library(${LIB} STATIC ${SOURCES})
target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}")

target_link_libraries(${LIB} PRIVATE
   Util
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Network
   OpenSSL::SSL
)
