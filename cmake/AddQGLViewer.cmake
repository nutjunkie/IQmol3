# find_package(QGLViewer)
# if(NOT ${QGLVIEWER_FOUND})
#    FetchContent_Declare(
#       libQGLViewer
#       GIT_REPOSITORY https://github.com/GillesDebunne/libQGLViewer.git
#       GIT_TAG 45a4e92aa60f3c33eed948b6f421e696a2a171f4
#    )
# endif(NOT ${QGLVIEWER_FOUND})
include_directories(modules/libQGLViewer)
add_subdirectory(modules EXCLUDE_FROM_ALL)
