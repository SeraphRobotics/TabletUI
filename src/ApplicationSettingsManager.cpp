#include "ApplicationSettingsManager.h"

#include <QSettings>
#include <QGuiApplication>
#include <QDebug>
#include <QFile>

/*!
 * \class ApplicationSettingsManager
 * \brief Class to store all application data, settings, file paths
 */

ApplicationSettingsManager::ApplicationSettingsManager(QObject *parent) :
    QObject(parent)
{
#ifdef Q_OS_WIN
    m_ApplicationIniFilePath = static_cast<QString>
            (getenv("APPDATA")) + "/Seraph/SeraphData.ini";
#endif

#ifdef Q_OS_LINUX
    m_ApplicationIniFilePath = qApp->applicationDirPath() + "/settings.ini";
#endif

    if (QFile::exists(m_ApplicationIniFilePath))
    {
        _readSettingsFile();
    }
    else
    {
        _createSettingsFile();
    }
}

ApplicationSettingsManager::~ApplicationSettingsManager()
{
}

ApplicationSettingsManager& ApplicationSettingsManager::getInstance()
{
    static ApplicationSettingsManager instance;
    return instance;
}

/*!
 * \brief Perform settings file creation
 */
void ApplicationSettingsManager::_createSettingsFile()
{
    qDebug() << QString("Not found settings file in %1, create now")
                .arg(m_ApplicationIniFilePath);

    QSettings settings(m_ApplicationIniFilePath, QSettings::IniFormat);
    settings.setValue("scan-extension", "scan");
    settings.setValue("ortho-extension", "ortho");
    settings.setValue("patient-file",
                      qApp->applicationDirPath().append("/patients.xml"));
    settings.setValue("users-file",
                      qApp->applicationDirPath().append("/users.xml"));

    settings.sync();
    _readSettingsFile();
}

/*!
 * \brief Read data from settings file
 */
void ApplicationSettingsManager::_readSettingsFile()
{
    QSettings settings(m_ApplicationIniFilePath, QSettings::IniFormat);

    m_PatientsFilePath = settings.value("patient-file").toString();
    m_UsersFilePath = settings.value("users-file").toString();
    m_ScanFilePath = settings.value("scan-file").toString();
    m_OrderMaterialsUrl = settings.value("orderMaterialsUrl").toString();
    m_ContactSupportUrl = settings.value("contactSupportUrl").toString();
    m_PinRequire = settings.value("pinRequire").toBool();
    m_AutoAssignPatients = settings.value("autoAssignPatients").toBool();
    m_LoginByPrescriber = settings.value("loginByPrescriber").toBool();
    m_Slic3rBinPath = settings.value("slic3r_bin_path").toString();
}

/*!
 * \brief Return users file path
 * \return
 */
const QString& ApplicationSettingsManager::usersFilePath() const
{
    return m_UsersFilePath;
}

/*!
 * \brief Return patients file path
 * \return
 */
const QString& ApplicationSettingsManager::patientsFilePath() const
{
    return m_PatientsFilePath;
}

/*!
 * \brief Set starting state
 */
void ApplicationSettingsManager::setStartingState() const
{
    QSettings settings(m_ApplicationIniFilePath, QSettings::IniFormat);

    if (settings.value("first-run").isNull() == true)
    {
        settings.setValue("first-run",false);
        emit setStateOnApplicationStart("welcomeScreenState");
    }
    else
    {
        emit setStateOnApplicationStart("startScreenState");
    }
}

/*!
 * \brief Return order materials url
 * \return
 */
const QString ApplicationSettingsManager::orderMaterialsUrl() const
{
    return m_OrderMaterialsUrl;
}

/*!
 * \brief Return contact support url
 * \return
 */
const QString ApplicationSettingsManager::contactSupportUrl() const
{
    return m_ContactSupportUrl;
}

/*!
 * \brief Return scan file path
 * \return
 */
const QString ApplicationSettingsManager::scanFilePath() const
{
    return m_ScanFilePath;
}

/*!
 * \brief Save account settings to *.ini file
 * \param pinRequire
 * \param loginByPrescriber
 * \param autoAssignPatients
 */
void ApplicationSettingsManager::saveAccountSettingsToIniFile(const bool& pinRequire,
                                                              const bool& loginByPrescriber,
                                                              const bool& autoAssignPatients)
{
    QSettings settings(m_ApplicationIniFilePath, QSettings::IniFormat);

    settings.setValue("pinRequire",pinRequire);
    settings.setValue("loginByPrescriber", loginByPrescriber);
    settings.setValue("autoAssignPatients", autoAssignPatients);

    setLoginByPrescriber(loginByPrescriber);
    setAutoAssignPatients(autoAssignPatients);
    setPinRequire(pinRequire);
}

/*!
 * \brief Return whether app should auto assign patients
 * \return
 */
bool ApplicationSettingsManager::autoAssignPatients() const
{
    return m_AutoAssignPatients;
}

/*!
 * \brief Return whether app should login by prescriber
 * \return
 */
bool ApplicationSettingsManager::loginByPrescriber() const
{
    return m_LoginByPrescriber;
}

/*!
 * \brief Return whether pin is required
 * \return
 */
bool ApplicationSettingsManager::pinRequire() const
{
    return m_PinRequire;
}

/*!
 * \brief Set whether app should auto assign patients
 * \param autoAssign
 */
void ApplicationSettingsManager::setAutoAssignPatients(const bool& autoAssign)
{
    if (m_AutoAssignPatients != autoAssign)
    {
        m_AutoAssignPatients = autoAssign;
        emit sigAutoAssignPatientsChanged();
    }
}

/*!
 * \brief Set whether app should login by prescriber
 * \param loginByPrescriber
 */
void ApplicationSettingsManager::setLoginByPrescriber(const bool& loginByPrescriber)
{
    if (m_LoginByPrescriber != loginByPrescriber)
    {
        m_LoginByPrescriber = loginByPrescriber;
        emit sigLoginByPrescriberChanged();
    }
}

/*!
 * \brief Set whether pin is required
 * \param pinRequire
 */
void ApplicationSettingsManager::setPinRequire(const bool& pinRequire)
{
    if (m_PinRequire != pinRequire)
    {
        m_PinRequire = pinRequire;
        emit sigPinRequireChanged();
    }
}

/*!
 * \brief Return full path to Slic3r bin folder
 * \return
 */
QString ApplicationSettingsManager::slic3rBinPath() const
{
    return m_Slic3rBinPath;
}
