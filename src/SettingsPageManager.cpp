#include "SettingsPageManager.h"

/*!
 * \class SettingsPageManager
 * \brief Class to store the states of SettingsPage children items
 */

SettingsPageManager::SettingsPageManager(QObject *parent) :
    QObject(parent),
    m_CurrentSelectedDirection("left"),
    m_ShellModificationsState("up")
{
}

/*!
 * \brief Return main settings page state
 * \return
 */
QString SettingsPageManager::mainSettingsPageState() const
{
    return m_MainSettingsPageState;
}

/*!
 * \brief Return current selected direction
 * \return
 */
QString SettingsPageManager::currentSelectedDirection() const
{
    return m_CurrentSelectedDirection;
}

/*!
 * \brief Return shell modification state
 * \return
 */
QString SettingsPageManager::shellModificationsState() const
{
    return m_ShellModificationsState;
}

/*!
 * \brief Return modification state
 * \return
 */
QString SettingsPageManager::modificationState() const
{
    return m_ModificationPageState;
}

/*!
 * \brief Set main settings page state
 * \param mainSettingsPageState
 */
void SettingsPageManager::setMainSettingsPageState(const QString &mainSettingsPageState)
{
    if (m_MainSettingsPageState != mainSettingsPageState)
    {
        m_MainSettingsPageState = mainSettingsPageState;
        emit sigMainSettingsPageStateChanged();
    }
}

/*!
 * \brief Set current selected direction
 * \param currentSelectedDirection
 */
void SettingsPageManager::setCurrentSelectedDirection(const QString &currentSelectedDirection)
{
    if (m_CurrentSelectedDirection != currentSelectedDirection)
    {
        m_CurrentSelectedDirection = currentSelectedDirection;
        emit sigCurrentSelectedDirectionChanged();
    }
}

/*!
 * \brief Set shell modification state
 * \param shellModificationsState
 */
void SettingsPageManager::setShellModificationsState(const QString &shellModificationsState)
{
    if (m_ShellModificationsState != shellModificationsState)
    {
        m_ShellModificationsState = shellModificationsState;
        emit sigShellModificationsStateChanged();
    }
}

/*!
 * \brief Set modification page state
 * \param modificationState
 */
void SettingsPageManager::setModificationPageState(const QString &modificationState)
{
    if (m_ModificationPageState != modificationState)
    {
        m_ModificationPageState = modificationState;
        emit sigModificationStateChanged();
    }
}
