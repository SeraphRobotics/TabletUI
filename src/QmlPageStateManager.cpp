#include "QmlPageStateManager.h"

#include "ApplicationSettingsManager.h"

#include <QDebug>

/*!
 * \class QmlPageStateManager
 * \brief Class used to switch between screens
 */

QmlPageStateManager::QmlPageStateManager(QObject *parent) :
    QObject(parent)
{
    connect(&ApplicationSettingsManager::getInstance(),
            &ApplicationSettingsManager::setStateOnApplicationStart,
            this,
            &QmlPageStateManager::setState);
}

/*!
 * \brief Set new state
 * \param newState
 */
void QmlPageStateManager::setState(const QString &newState)
{
    qDebug() << "new state:" << newState;

    if (newState.isEmpty() || newState == m_CurrentState)
    {
        qDebug() << "Warning: state name is empty,"
                    "duplicated, or popup open, this should not happen:)";
        return;
    }

    m_PreviousState = m_CurrentState;
    m_CurrentState = newState;
    emit sigSwitchToSpecificScreen(newState);
}

/*!
 * \brief Return previous state
 * \return
 */
QString QmlPageStateManager::previousState() const
{
    qDebug() << "previous state:" << m_PreviousState;
    return m_PreviousState;
}

