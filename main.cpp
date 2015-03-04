#include <QtGui/QGuiApplication>
#include <QQmlContext>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlEngine>



#include "qtquick2applicationviewer.h"
#include "src/QmlPageStateManager.h"
#include "src/QmlCppWrapper.h"
#include "src/ApplicationSettingsManager.h"
#include "src/Models/UsersListModel.h"
#include "src/SettingsPageManager.h"

#include "3dHelper/q3dhelper.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName("Seraph");
    QGuiApplication::setOrganizationDomain("seraphrobotics.com");
    QGuiApplication::setApplicationName("SeraphData");

    QtQuick2ApplicationViewer viewer;

    QmlCppWrapper cppWrapper;
    QmlPageStateManager stateManager;
    SettingsPageManager settingsPageManager;

    viewer.rootContext()->setContextProperty("stateManager",
                                             &stateManager);
    viewer.rootContext()->setContextProperty("qmlCppWrapper",
                                             &cppWrapper);
    viewer.rootContext()->setContextProperty("applicationSettings",
                                             &ApplicationSettingsManager::getInstance());
    viewer.rootContext()->setContextProperty("_mainWindow",
                                             &viewer);
    viewer.rootContext()->setContextProperty("settingsPageManager",
                                             &settingsPageManager);

    // For View3d
    qmlRegisterType<Q3DHelper>("Helpers", 1, 0, "Q3DHelper");

    viewer.setMainQmlFile(QStringLiteral("qml/SeraphRoboticsUi/main.qml"));
    //viewer.setMainQmlFile(QStringLiteral("qml/SeraphRoboticsUi/RegularScreens/StartPage.qml"));
    viewer.showExpanded();

    ApplicationSettingsManager::getInstance().setStartingState();

    return app.exec();
}
