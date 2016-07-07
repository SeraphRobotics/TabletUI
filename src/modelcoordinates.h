#ifndef MODELCOORDINATES_H
#define MODELCOORDINATES_H

#include <QObject>
#include <QPointF>
#include <QSizeF>
#include <QVector3D>

class ModelCoordinates : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal cameraAngle READ cameraAngle WRITE setCameraAngle NOTIFY cameraAngleChanged)
    Q_PROPERTY(qreal cameraRatio READ cameraRatio NOTIFY cameraRatioChanged)
    Q_PROPERTY(qreal cameraHeight READ cameraHeight NOTIFY cameraHeightChanged)
    Q_PROPERTY(QPointF cameraShift READ cameraShift NOTIFY cameraShiftChanged)
    Q_PROPERTY(QVector3D cameraPosition READ cameraPosition NOTIFY cameraPositionChanged)
    Q_PROPERTY(QVector3D cameraCenter READ cameraCenter NOTIFY cameraCenterChanged)

public:
    ModelCoordinates(QObject *parent, qreal angle, QPointF pos, QSizeF scene,
                     QSizeF scan, QSizeF window, qreal z);

    qreal cameraAngle() const;
    qreal cameraRatio() const;
    qreal cameraHeight() const;
    const QPointF& cameraShift() const;
    QVector3D cameraPosition() const;
    QVector3D cameraCenter() const;
    const QPointF& scenePos() const;
    const QSizeF& sceneSize() const;
    const QSizeF& windowSize() const;
    qreal z() const;

    void setCameraAngle(qreal angle);
    Q_INVOKABLE void setScenePos(QPointF pos);
    Q_INVOKABLE void setSceneSize(QSizeF size);
    Q_INVOKABLE void setWindowSize(QSizeF size);
    Q_INVOKABLE void setAreaSize(QSizeF size);
    void setZ(qreal z);

    QPointF model2ui(const QPointF& model);
    QPointF ui2model(const QPointF& ui);

    Q_INVOKABLE void applyChanges();

signals:
    void cameraAngleChanged() const;
    void cameraRatioChanged() const;
    void cameraPositionChanged() const;
    void cameraCenterChanged() const;
    void cameraShiftChanged() const;
    void cameraHeightChanged() const;

private:
    void updateCoefficients();

    qreal angle;
    QPointF pos;
    QSizeF scene;
    QSizeF area;
    QSizeF window;
    qreal pos_z;

    bool coefficientsAreValid = false;
    QPointF shift;
    qreal ratio;
    qreal dx, dy;
    qreal cx, cy, cz;
    qreal y_offset;
    qreal a1, a2, b1, b2;
};

#endif // MODELCOORDINATES_H
