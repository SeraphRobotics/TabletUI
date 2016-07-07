#ifndef FOOT_H
#define FOOT_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QVariantList>

#include "libraries/shared/fah-constants.h"
#include "DataStructures/manipulations.h"

class ControlPoints;
class ImageContoursDetector;

class Foot : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage scanImage READ getScanImage NOTIFY scanImageGenerated)
    Q_PROPERTY(QObject* controlPoints READ controlPoints CONSTANT)
    Q_PROPERTY(qreal canvasScale READ canvasScale WRITE setCanvasScale NOTIFY canvasScaleChanged)

public:
    explicit Foot(QObject *parent = 0);
    ~Foot();

    Q_INVOKABLE void addModification(const int &type,
                                     const double &depth,
                                     const double &height,
                                     const double &stiffness,
                                     const QImage &image,
                                     const int &startingXPos,
                                     const int &startingYPos,
                                     const QSizeF &canvasSize);
    QVariantList getModifications(const QVector<Manipulation *> &manipulationsVector,
                                  const QSizeF &canvasSize) const;

    void setScanImage(const QImage &image);
    QImage getScanImage() const;
    Q_INVOKABLE qreal scanWidth() const;
    Q_INVOKABLE qreal scanHeight() const;
    Q_INVOKABLE QVariantList getBoundaryPoints(const qreal &canvasScale) const;
    QObject *controlPoints() const;
    void calculateInitialPoints(qreal viewScale);
    void recalculateBoundary(const QVariantList &userPoints);
    void setBoundaryPoints(const QVector<FAHVector3> &points);

    QPointF grid2ui(qreal x, qreal y) const;
    QPointF grid2ui(qreal x, qreal y, const qreal &canvasScale) const;
    FAHVector3 ui2grid(qreal x, qreal y) const;
    FAHVector3 ui2grid(qreal x, qreal y, const qreal &canvasScale) const;

    void setScanId(const QString &scanId);
    QString scanId() const;
    void setOrthoticId(const QString &orthoticId);
    QString orthoticId() const;

    void setForePoints(const QVector<FAHVector3> &forePoints);
    QVector<FAHVector3> forePoints() const;
    void setHealPoints(const QVector<FAHVector3> &healPoints);
    QVector<FAHVector3> healPoints() const;

    Posting *forePosting() const;
    Posting *healPosting() const;
    Top_Coat *topCoat() const;
    QVector<Manipulation*> manipulations() const;
    Q_INVOKABLE void applyUserChanges(const QList<qreal> &postingChanges,
                                      const QList<qreal> &topcoatChanges);

    Q_INVOKABLE QString stlModelFile() const;
    void setStlModelFile(const QString &stlModelFile);

    qreal canvasScale() const;
    void setCanvasScale(const qreal &canvasScale);

    void clean();
    void cleanModificationsData();

private:
    QVector<qreal> m_BoundaryPoints;
    QVector<FAHVector3> m_ForePoints;
    QVector<FAHVector3> m_HealPoints;
    ControlPoints *m_ControlPoints = nullptr;

    QImage m_ScanImage;
    QString m_ScanId;
    QString m_OrthoticId;
    ImageContoursDetector *m_ImageContoursDetector = nullptr;

    // need to remove manipulations list
    QVector<Manipulation*> m_Manipulations;

    Posting *m_ForePosting = nullptr;
    Posting *m_HealPosting = nullptr;
    Top_Coat *m_TopCoat = nullptr;
    QString m_StlModelFile;

    qreal m_CanvasScale;

signals:
    void forePointsChanged() const;
    void healPointsChanged() const;
    void boundaryPointsChanged() const;
    void scanImageGenerated() const;
    void boundaryUpdated() const;
    void canvasScaleChanged() const;
};

#endif // FOOT_H
