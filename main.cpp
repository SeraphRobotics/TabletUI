#include <QtGui/QGuiApplication>
#include <QSettings>
#include <QDir>

#include "src/ApplicationSettingsManager.h"
#include "applicationview.h"
#include "FileLocationHelper.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName("Seraph");
    QGuiApplication::setOrganizationDomain("seraphrobotics.com");
    QGuiApplication::setApplicationName("SeraphData");

    auto fileLocationHelper = FileLocationHelper::instance();
    // For searching the scans list into Qt resources
    QSettings settings;
    settings.setValue("scan-directory", fileLocationHelper->getScanDir());
    settings.setValue("ortho-directory", fileLocationHelper->getOrthoDir());
    // TODO: ask the user necessary path to binary
    settings.setValue("printing/slicer", ApplicationSettingsManager::getInstance().slic3rBinPath());
    settings.setValue("printing/plastic_ini", qApp->applicationDirPath() + "/p.ini");
    settings.setValue("printing/directory", qApp->applicationDirPath());
    settings.setValue("printing/inis", qApp->applicationDirPath());
    settings.setValue("printing/valving-python-script", qApp->applicationDirPath() + "/toValve.py");
    settings.setValue("printing/merge-python-script", qApp->applicationDirPath() + "/merge.py");
    // Path where will be saved scans from scanner device
    settings.setValue("scanner/directory", QDir::tempPath());

    ApplicationView view;
    view.setSource(QUrl("qrc:/qml/SeraphRoboticsUi/main.qml"));
    view.showExpanded();

    ApplicationSettingsManager::getInstance().setStartingState();

    return app.exec();
}
