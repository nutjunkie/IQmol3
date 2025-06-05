find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)
set(LIB Util)

set(HEADERS
   Color.h
   ColorDialog.h
   QMsgBox.h
   LogMessageDialog.h
   ProgressDialog.h
   Task.h
   Timer.h
   Zip.h
)

set( SOURCES
   Color.C
   ColorDialog.C
   ColorGradient.C
   ColorGradientDialog.C
   FileDialog.C
   GLShape.C
   GLShapeLibrary.C
   LogMessageDialog.C
   Preferences.C
   ProgressDialog.C
   QMsgBox.C
   QsDebugOutput.C
   QsLog.C
   QsLogDest.C
   RemoveDirectory.C
   ScanDirectory.C
   SetButtonColor.C
   StatusWidget.C
   Task.C
   Timer.C
   WaitingSpinner.C
   WriteToTemporaryFile.C
   Zip.C
)

set( UI_FILES
   ColorDialog.ui
   LogMessageDialog.ui
   ProgressDialog.ui
)

qt5_wrap_ui(UI_HEADERS ${UI_FILES} )

qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} STATIC ${SOURCES} ${UI_HEADERS})
target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
   #QGLViewer
   #OpenGL::GL
   archive
)
