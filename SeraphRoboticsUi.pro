# Add more folders to ship with the application, here
folder_01.source = qml/SeraphRoboticsUi
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

QT += xml 3dquick core qml

QMAKE_CXXFLAGS += -std=c++11


INCLUDEPATH +=  src/ \
                src/XmlManager/

INCLUDEPATH += ../seraphLibs/
#INCLUDEPATH += /usr/include/opencv
#INCLUDEPATH += /usr/include/opencv2

LIBS += -L../build-seraphLibs/
LIBS += -lseraphLibs

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    src/QmlPageStateManager.cpp \
    src/QmlCppWrapper.cpp \
    src/Models/UserObject.cpp \
    src/Models/PatientObject.cpp \
    src/Models/NameObject.cpp \
    src/Models/UsersListModel.cpp \
    src/Models/PatientsListModel.cpp \
    src/ApplicationSettingsManager.cpp \
    src/Models/PatientDataObject.cpp \
    src/SettingsPageManager.cpp \
    src/XmlManager/XmlFileManager.cpp \
    src/3dHelper/q3dhelper.cpp \
    src/ImageProcessingClass/ImageContoursDetector.cpp \
    src/mastercontrolunit.cpp


# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    src/QmlPageStateManager.h \
    src/QmlCppWrapper.h \
    src/Models/UserObject.h \
    src/Models/PatientObject.h \
    src/Models/NameObject.h \
    src/Models/UsersListModel.h \
    src/Models/PatientsListModel.h \
    src/ApplicationSettingsManager.h \
    src/Models/PatientDataObject.h \
    src/SettingsPageManager.h \
    src/XmlManager/XmlFileManager.h \
    src/3dHelper/q3dhelper.h \
    src/ImageProcessingClass/ImageContoursDetector.h \
    src/mastercontrolunit.h

RESOURCES += \
    resourcesFile.qrc

##This part is for building exe with administrator acess, use only for build exe for user.

#win32 {
#CONFIG += SeraphRoboticsUi.exe.manifest
#QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'
#}

win32:RC_FILE = SeraphRoboticsUi.rc
win32:CONFIG(release, debug|release): LIBS +=-L$$PWD/../../../../opencv/build/x86/vc12/lib/ -lopencv_core249
else:win32:CONFIG(debug, debug|release): LIBS +=-L$$PWD/../../../../opencv/build/x86/vc12/lib/ -lopencv_core249d
unix:LIBS += -lopencv_core

win32:INCLUDEPATH += $$PWD/../../../../opencv/build/include
win32:DEPENDPATH += $$PWD/../../../../opencv/build/include

win32:CONFIG(release, debug|release): LIBS +=-L$$PWD/../../../../opencv/build/x86/vc12/lib/ -lopencv_imgproc249
else:win32:CONFIG(debug, debug|release): LIBS +=-L$$PWD/../../../../opencv/build/x86/vc12/lib/ -lopencv_imgproc249d
unix:LIBS += -lopencv_imgproc

win32:INCLUDEPATH += $$PWD/../../../../opencv/build/include
win32:DEPENDPATH += $$PWD/../../../../opencv/build/include

win32:CONFIG(release, debug|release): LIBS +=-L$$PWD/../../../../opencv/build/x86/vc12/lib/ -lopencv_highgui249
else:win32:CONFIG(debug, debug|release): LIBS +=-L$$PWD/../../../../opencv/build/x86/vc12/lib/ -lopencv_highgui249d
unix:LIBS += -lopencv_highgui

win32:INCLUDEPATH += $$PWD/../../../../opencv/build/include
win32:DEPENDPATH += $$PWD/../../../../opencv/build/include
