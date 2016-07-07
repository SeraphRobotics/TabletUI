#include "applicationview.h"

#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include "src/QmlPageStateManager.h"
#include "src/QmlCppWrapper.h"
#include "src/ApplicationSettingsManager.h"
#include "src/Models/UsersListModel.h"
#include "src/Models/PatientsListModel.h"
#include "src/SettingsPageManager.h"
#include "ImageProvider.h"
#include "ManipulationData.h"

#ifdef Q_OS_LINUX
#   include <X11/XKBlib.h>
#endif

ApplicationView::ApplicationView() : QQuickView()
{
    if (qgetenv("QT_QUICK_CORE_PROFILE").toInt()) {
        QSurfaceFormat f = format();
        f.setProfile(QSurfaceFormat::CoreProfile);
        f.setVersion(4, 4);
        setFormat(f);
    }

    setResizeMode(QQuickView::SizeViewToRootObject);

    m_defaultMaximumSize = maximumSize();
    /// @note after discussion with Adam we set this as our minium working size
    /// 1280-800 this is minimal working resolution.(Finall devices resolution as well.)
    m_normalMinimumSize = QSize(1280, 800);
    unlockResize();

    connect(engine(), &QQmlEngine::quit, this, &QQuickView::close);
    connect(this, &QQuickView::activeChanged, this, &ApplicationView::_windowsActiveChanged);

    // NOTE: We can apply it if needed further
    // new QQmlFileSelector(view.engine(), &view);

    QmlCppWrapper *cppWrapper = new QmlCppWrapper(this);
    auto imageProvider = new ImageProvider(cppWrapper);
    engine()->addImageProvider("scan_images", imageProvider);
    QmlPageStateManager *stateManager = new QmlPageStateManager(cppWrapper);
    SettingsPageManager *settingsPageManager = new SettingsPageManager(cppWrapper);

    connect(cppWrapper, &QmlCppWrapper::footSideChanged,
            settingsPageManager, &SettingsPageManager::setCurrentSelectedDirection);

    rootContext()->setContextProperty("stateManager", stateManager);
    rootContext()->setContextProperty("patientsListModel",
                                      QVariant::fromValue(cppWrapper->getPatientsList()));
    rootContext()->setContextProperty("usersListModel",
                                      QVariant::fromValue(cppWrapper->getUsersList()));
    rootContext()->setContextProperty("qmlCppWrapper", cppWrapper);
    rootContext()->setContextProperty("applicationSettings", &ApplicationSettingsManager::getInstance());
    rootContext()->setContextProperty("_mainWindow", this);
    rootContext()->setContextProperty("settingsPageManager", settingsPageManager);

    qmlRegisterUncreatableType<Manipulation>("Manipulation", 1, 0, "Manipulation", "Manipulation class can not be created");
    qmlRegisterUncreatableType<ManipulationData>("ManipulationData",
                                                 1,
                                                 0,
                                                 "ManipulationData",
                                                 "ManipulationData class cannot be created in QML");
}

void ApplicationView::showExpanded()
{
#if defined(Q_WS_SIMULATOR) || defined(Q_OS_QNX)
    showFullScreen();
#else
    show();
#endif
}

void ApplicationView::lockResize()
{
    qDebug()<<"Lock resize";
    setMaximumSize(QSize(width(), height()));
    setMinimumSize(QSize(width(), height()));
}

void ApplicationView::unlockResize()
{
    setMaximumSize(m_defaultMaximumSize);
    setMinimumSize(m_normalMinimumSize);
}

bool ApplicationView::capsLockPressed() const
{
    return m_CapsLockPressed;
}

void ApplicationView::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_CapsLock)
    {
        _setCapsLockPressed();
    }

    QQuickView::keyPressEvent(e);
}

void ApplicationView::_setCapsLockPressed()
{
    auto checkCapsLockState = [] ()
    {
        // platform dependent method of determining if CAPS LOCK is on
#ifdef Q_OS_WIN // MS Windows version
        return GetKeyState(VK_CAPITAL) == 1;
#else // X11 version (Linux/Unix/Mac OS X/etc...)
        Display * d = XOpenDisplay((char*)0);
        bool caps_state = false;
        if (d)
        {
            unsigned n;
            XkbGetIndicatorState(d, XkbUseCoreKbd, &n);
            caps_state = (n & 0x01) == 1;
        }
        return caps_state;
#endif
    };

    m_CapsLockPressed = checkCapsLockState();
    qDebug()<<"Check caps lock pressed state, current is :"<<m_CapsLockPressed;
    emit sigCapsLockPressedChanged();
}

void ApplicationView::_windowsActiveChanged()
{
    if (isActive())
        _setCapsLockPressed();
}

