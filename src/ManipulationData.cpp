#include "ManipulationData.h"

/*!
 * \class ManipulationData
 * \brief Class contains all manipulation data
 */

/*!
 * \brief Set manipulation type
 * \param type
 */
void ManipulationData::setType(Manipulation::ManipulationType type)
{
    m_Type = type;
}

/*!
 * \brief Return manipulation type
 * \return
 */
Manipulation::ManipulationType ManipulationData::type() const
{
    return m_Type;
}

/*!
 * \brief Set manipulation location
 * \param point
 */
void ManipulationData::setLocation(const QPoint &point)
{
    m_Location.setX(point.x());
    m_Location.setY(point.y());
}

/*!
 * \brief Return manipulation location
 * \return
 */
QPoint ManipulationData::location() const
{
    return m_Location;
}

/*!
 * \brief Set manipulation stiffness
 * \param stiffness
 */
void ManipulationData::setStiffness(qreal stiffness)
{
    m_Stiffness = stiffness;
}

/*!
 * \brief Return manipulation stiffness
 * \return
 */
qreal ManipulationData::stiffness() const
{
    return m_Stiffness;
}

/*!
 * \brief Set manipulation depth
 * \param depth
 */
void ManipulationData::setDepth(qreal depth)
{
    m_Depth = depth;
}

/*!
 * \brief Return manipulation depth
 * \return
 */
qreal ManipulationData::depth() const
{
    return m_Depth;
}

/*!
 * \brief Set manipulation thickness
 * \param thickness
 */
void ManipulationData::setThickness(qreal thickness)
{
    m_Thickness = thickness;
}

/*!
 * \brief Return manipulation thickness
 * \return
 */
qreal ManipulationData::thickness() const
{
    return m_Thickness;
}

