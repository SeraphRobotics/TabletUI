#include "ControlPoints.h"

#include <QPointF>

/*!
 * \class ControlPoints
 * \brief Class is a container for heal and fore foot points
 */

ControlPoints::ControlPoints(QObject *parent) :
    QObject(parent)
{
}

/*!
 * \brief Fore point for foot
 * \return
 */
QVariantList ControlPoints::forePoints() const
{
    return m_ForePoints;
}

void ControlPoints::setForePoints(const QVariantList &forePoints)
{
    if (m_ForePoints != forePoints) {
        m_ForePoints = forePoints;

        emit forePointsChanged();
    }
}

/*!
 * \brief heal points for foot
 * \return
 */
QVariantList ControlPoints::healPoints() const
{
    return m_HealPoints;
}

void ControlPoints::setHealPoints(const QVariantList &healPoints)
{
    if (m_HealPoints != healPoints) {
        m_HealPoints = healPoints;

        emit healPointsChanged();
    }
}
