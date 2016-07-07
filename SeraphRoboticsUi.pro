# MILOSOLUTIONS @ 2016
# Project: SeraphRobotics
#
# List of environment viariables that must be set:
#   opencv_inc   - path to headers for OpenCV library
#   opencv_lib   - path to OpenCV libraries ver 2.4
#   backend_lib  - SR backend static libary path
#
# Optional variables:
#   USE_FAKE_SCANNER - instead of real scanner device use virtual dummy object (test only)
#   USE_EXISTING_STL - don't calculate any stl file, use those existing in directory (test only)

TEMPLATE = app
QT += xml quick qml core serialport
# For Qt3D
QT += 3dcore 3dquick 3drenderer 3dinput

CONFIG += c++11

include(conf.pri)

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    src/QmlPageStateManager.cpp \
    ../SeraphRoboticsBackend/DataStructures/basicstructures.cpp \
    ../SeraphRoboticsBackend/DataStructures/manipulations.cpp \
    ../SeraphRoboticsBackend/DataStructures/orthotic.cpp \
    ../SeraphRoboticsBackend/DataStructures/orthoticmanager.cpp \
    ../SeraphRoboticsBackend/DataStructures/patientdata.cpp \
    ../SeraphRoboticsBackend/DataStructures/patientmanager.cpp \
    ../SeraphRoboticsBackend/DataStructures/scan.cpp \
    ../SeraphRoboticsBackend/DataStructures/scanmanager.cpp \
    ../SeraphRoboticsBackend/DataStructures/test.cpp \
    ../SeraphRoboticsBackend/DataStructures/user.cpp \
    ../SeraphRoboticsBackend/DataStructures/usermanager.cpp \
    ../SeraphRoboticsBackend/DataStructures/xygrid.cpp \
    src/QmlCppWrapper.cpp \
    src/Models/UserObject.cpp \
    src/Models/PatientObject.cpp \
    src/Models/NameObject.cpp \
    src/Models/UsersListModel.cpp \
    src/Models/PatientsListModel.cpp \
    src/ApplicationSettingsManager.cpp \
    src/Models/PatientDataObject.cpp \
    src/SettingsPageManager.cpp \
    src/ImageProvider.cpp \
    applicationview.cpp \
    src/Workers/ModelGeneratorWorker.cpp \
    src/FileLocationHelper.cpp \
    src/fakescannercontroller.cpp \
    src/ManipulationData.cpp \
    src/modelcoordinates.cpp \
    src/Models/Foot.cpp \
    src/Models/ControlPoints.cpp \
    src/Models/UsbDataModel.cpp \
    src/Workers/TransferRxWorker.cpp \
    src/Workers/GcodeGeneratorWorker.cpp \
    logger.cpp

INCLUDEPATH += ../SeraphRoboticsBackend \
               ../SeraphRoboticsBackend/DataStructures/ \
                src/

HEADERS += \
    src/QmlPageStateManager.h \
    ../SeraphRoboticsBackend/DataStructures/basicstructures.h \
    ../SeraphRoboticsBackend/DataStructures/manipulations.h \
    ../SeraphRoboticsBackend/DataStructures/orthotic.h \
    ../SeraphRoboticsBackend/DataStructures/orthoticmanager.h \
    ../SeraphRoboticsBackend/DataStructures/patientdata.h \
    ../SeraphRoboticsBackend/DataStructures/patientmanager.h \
    ../SeraphRoboticsBackend/DataStructures/scan.h \
    ../SeraphRoboticsBackend/DataStructures/scanmanager.h \
    ../SeraphRoboticsBackend/DataStructures/test.h \
    ../SeraphRoboticsBackend/DataStructures/user.h \
    ../SeraphRoboticsBackend/DataStructures/usermanager.h \
    ../SeraphRoboticsBackend/DataStructures/xygrid.h \
    src/QmlCppWrapper.h \
    src/Models/UserObject.h \
    src/Models/PatientObject.h \
    src/Models/NameObject.h \
    src/Models/UsersListModel.h \
    src/Models/PatientsListModel.h \
    src/ApplicationSettingsManager.h \
    src/Models/PatientDataObject.h \
    src/SettingsPageManager.h \
    src/ImageProvider.h \
    applicationview.h \
    src/Workers/ModelGeneratorWorker.h \
    src/FileLocationHelper.h \
    src/fakescannercontroller.h \
    src/ManipulationData.h \
    src/modelcoordinates.h \
    src/Models/Foot.h \
    src/Models/ControlPoints.h \
    src/Models/UsbDataModel.h \
    src/Workers/TransferRxWorker.h \
    src/Workers/GcodeGeneratorWorker.h \
    logger.h


RESOURCES += \
    resourcesFile.qrc \
    qml.qrc


##This part is for building exe with administrator acess, use only for build exe for user.

#win32 {
#CONFIG += SeraphRoboticsUi.exe.manifest
#QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'
#}

win32:RC_FILE = SeraphRoboticsUi.rc
