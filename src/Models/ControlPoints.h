#ifndef CONTROLPOINTS_H
#define CONTROLPOINTS_H

#include <QObject>
#include <QVariantList>

class ControlPoints : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList forePoints READ forePoints WRITE setForePoints NOTIFY forePointsChanged)
    Q_PROPERTY(QVariantList healPoints READ healPoints WRITE setHealPoints NOTIFY healPointsChanged)

public:
    explicit ControlPoints(QObject *parent = 0);

    QVariantList forePoints() const;
    QVariantList healPoints() const;
    void setForePoints(const QVariantList &userPoints);
    void setHealPoints(const QVariantList &userPoints);

signals:
    void forePointsChanged() const;
    void healPointsChanged() const;

private:
    QVariantList m_ForePoints;
    QVariantList m_HealPoints;

};

#endif // CONTROLPOINTS_H
