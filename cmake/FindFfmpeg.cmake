#----------------------------------------------------------------
# ffmpeg
#----------------------------------------------------------------

#set( proj ffmpeg )
#set( SHARED_FFMPEG )
#
#set(BASIC
#   --extra-ldflags=-L/usr/local/lib
#   --extra-cflags=-I/usr/local/include
#   --enable-gpl
#   --enable-libx264
#)
#
#option( FFMPEG_GPL "Use a GPL version of FFMPEG" OFF )
#
#set(FFMPEG_SOURCE_DIR  ${CMAKE_SOURCE_DIR}/modules/ffmeg )
#set(FFMPEG_INSTALL_DIR ${CMAKE_BINARY_DIR}/modules/ffmeg )
#
#ExternalProject_Add(${proj}
#   SOURCE_DIR ${FFMPEG_SOURCE_DIR}
#   INSTALL_DIR ${FFMPEG_INSTALL_DIR}
#   # Build the project
#   BUILD_IN_SOURCE 0
#
#   # Configure step
#   # DO STH FOR THE ARCHITECTURE...
#   CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${FFMPEG_INSTALL_DIR} ${SHARED_FFMPEG} ${F#FMPEG_GPL_FLAG} ${BASIC}
#
#        #  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -j${NProcessors}
#)

# define the library suffix
#if( SUPER_SHARED_LIBS )
#    if( APPLE )
#        set(LIBRARY_SUFFIX .dylib)
#    else( APPLE )
#        set(LIBRARY_SUFFIX .so)
#    endif( APPLE )
#else( SUPER_SHARED_LIBS )
#    set(LIBRARY_SUFFIX .a)
#endif( SUPER_SHARED_LIBS )
#
## ADD DIRECTORIES FOR DEPENDENCIES IN Main/CMakeLists.txt
#set( FFMPEG_LIBRARYS libavcodec.a
#        libavformat.a
#        libavutil.a
#        libswscale.a
#        )
#
#include_directories(${FFMPEG_INSTALL_DIR}/include)
#
#set(FFMPEG_LIBRARY_DIRS ${FFMPEG_INSTALL_DIR}/lib/)
#link_directories(${FFMPEG_LIBRARY_DIRS})
