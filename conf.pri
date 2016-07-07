# Configuration file containing pathes to all required libraries for different
# OSes for backend.

OPENCV_INCLUDE = $$(opencv_inc)
INCLUDEPATH += "$$OPENCV_INCLUDE"

OPENCV_LIBS = $$(opencv_lib)
isEmpty(OPENCV_INCLUDE) | isEmpty(OPENCV_LIBS) {
  error(You didn\'t specify 3rd party opencv headers and/or library. \
        Please see https://git.milosolutions.com/qt/seraphrobotics/wikis/build)
}

BACKEND_LIBS = $$(backend_lib)
isEmpty(BACKEND_LIBS) {
  error(You didn\'t specify backend library. \
        Please see https://git.milosolutions.com/qt/seraphrobotics/wikis/build)
}

LIBS += -L"$$BACKEND_LIBS" -L"$$OPENCV_LIBS"
CONFIG(release, debug|release): LIBS += -lseraphLibs
CONFIG(debug, debug|release): LIBS += -lseraphLibsd

unix|macx: {
  LIBS += -lX11
}

win32 {
  CONFIG += opencv2411
  # Set opencv library version suffix
  opencv2411 {
    OPENCV_VER = "2411"
  }
  opencv245 {
    OPENCV_VER = "245"
  }
  opencv243 {
    OPENCV_VER = "243"
  }

  LIBS += \
      -lopencv_core"$$OPENCV_VER" \
      -lopencv_highgui"$$OPENCV_VER" \
      -lopencv_imgproc"$$OPENCV_VER"
}

unix {
  LIBS += \
      -lopencv_core \
      -lopencv_highgui \
      -lopencv_imgproc
}

# Instructions to keep build directories clean
CONFIG(release, debug|release) {
  DESTDIR = release
  OBJECTS_DIR = release/.obj
  MOC_DIR = release/.moc
  RCC_DIR = release/.rcc
  UI_DIR = release/.ui
}
CONFIG(debug, debug|release) {
  DESTDIR = debug
  OBJECTS_DIR = debug/.obj
  MOC_DIR = debug/.moc
  RCC_DIR = debug/.rcc
  UI_DIR = debug/.ui
}

message(---)
message(BACKEND_LIBS: $$BACKEND_LIBS)
message(OPENCV_INCLUDE: $$OPENCV_INCLUDE)
message(OPENCV_LIBS: $$OPENCV_LIBS)
message(Libs: $$LIBS)
message(Includes: $$INCLUDEPATH)
message(---)
