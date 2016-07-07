#ifndef MANIPULATIONDATA_H
#define MANIPULATIONDATA_H

#include <QObject>
#include <QPoint>

#include "manipulations.h"

class ManipulationData
{
    Q_GADGET

    Q_PROPERTY(Manipulation::ManipulationType type READ type WRITE setType)
    Q_PROPERTY(QPoint location READ location WRITE setLocation)
    Q_PROPERTY(qreal stiffness READ stiffness WRITE setStiffness)
    Q_PROPERTY(qreal depth READ depth WRITE setDepth)
    Q_PROPERTY(qreal thickness READ thickness WRITE setThickness)

public:
    void setType(Manipulation::ManipulationType type);
    Manipulation::ManipulationType type() const;

    void setLocation(const QPoint &point);
    QPoint location() const;

    void setStiffness(qreal stiffness);
    qreal stiffness() const;

    void setDepth(qreal depth);
    qreal depth() const;

    void setThickness(qreal thickness);
    qreal thickness() const;

private:
    Manipulation::ManipulationType m_Type;
    QPoint m_Location;
    qreal m_Stiffness;
    qreal m_Depth;
    qreal m_Thickness;
};

#endif // MANIPULATIONDATA_H
