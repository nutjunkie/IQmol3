find_package(Qt5 COMPONENTS Core Gui Widgets Xml OpenGL REQUIRED)
set(LIB Util)

set(HEADERS
   ColorGradient.h
   ColorGradientDialog.h
   Constants.h
   Debug.h
   Exception.h
   GLShape.h
   GLShapeLibrary.h
   OpenGL.h
   Preferences.h
   QMsgBox.h
   QVariantPtr.h
   QsDebugOutput.h
   QsLog.h
   QsLogDest.h
   QsLogLevel.h
   RemoveDirectory.h
   ScanDirectory.h
   SetButtonColor.h
   Spin.h
   StringFormat.h
   Task.h
   Timer.h
   WriteToTemporaryFile.h
)

set( SOURCES
   ColorGradient.C
   ColorGradientDialog.C
   GLShape.C
   GLShapeLibrary.C
   Preferences.C
   QMsgBox.C
   QsDebugOutput.C
   QsLog.C
   QsLogDest.C
   RemoveDirectory.C
   ScanDirectory.C
   SetButtonColor.C
   Task.C
   Timer.C
   WriteToTemporaryFile.C
)

set( UI_FILES
   ColorGradientDialog.ui
)

qt5_wrap_ui(UI_HEADERS ${UI_FILES} )
qt5_wrap_cpp(SOURCES ${HEADERS} )
add_library(${LIB} ${SOURCES} ${UI_HEADERS})
target_include_directories(${LIB} PUBLIC "${${LIB}_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${LIB} PRIVATE 
   Qt5::Core 
   Qt5::Gui 
   Qt5::Widgets
   Qt5::Xml
   Qt5::OpenGL
)
