#include "Foot.h"
#include "ControlPoints.h"
#include "ManipulationData.h"

#include "Controllers/ImageProcessingClass/ImageContoursDetector.h"

/*!
 * \class Foot
 * \brief Class contains all needed information about foot
 */

Foot::Foot(QObject *parent) :
    QObject(parent),
    m_CanvasScale(1.0)
{
    m_ImageContoursDetector = new ImageContoursDetector(this);
    m_ControlPoints = new ControlPoints(this);
}

Foot::~Foot()
{
    // free memory and clear manipulations list
    qDeleteAll(m_Manipulations);
    m_Manipulations.clear();

    delete m_HealPosting;
    delete m_ForePosting;
    delete m_TopCoat;
}

/*!
 * \brief Perform creating and adding pad modifications to orthotic
 * Method also detects outer and inner border by ImageContoursDetector class
 * \param type Pad type
 * \param depth Customized depth for specific pad
 * \param height Customized height for specific pad
 * \param stiffness Customized stiffness for specific pad
 * \param image Image that contains specific pad view area
 * \param startingXPos Starting x position within foot coordinate system
 * \param startingYPos Starting y position within foot coordinate system
 * \param canvasSize Size of canvas
 */
void Foot::addModification(const int &type,
                           const double &depth,
                           const double &height,
                           const double &stiffness,
                           const QImage &image,
                           const int &startingXPos,
                           const int &startingYPos,
                           const QSizeF &canvasSize)
{
    Manipulation *manipulation = new Manipulation;
    manipulation->type = static_cast<Manipulation::ManipulationType>(type);
    manipulation->depth = depth;
    manipulation->thickness = height;
    manipulation->stiffness = stiffness;

    manipulation->outerloop = new FAHLoopInXYPlane();
    manipulation->innerloops = QList<FAHLoopInXYPlane*>();

    QVariantList outerBorder;
    QVector<QVector<QPoint> > innerBorders;
    m_ImageContoursDetector->detectContours(image,
                                            innerBorders,
                                            outerBorder,
                                            startingXPos,
                                            startingYPos);

    qreal canvasScale = canvasSize.width() / m_ScanImage.width();
    // Fill outer loop
    for (int i = 0; i < outerBorder.length(); i++) {
        QPoint point = outerBorder.at(i).toPoint();
        manipulation->outerloop->add(ui2grid(point.x(), point.y(), canvasScale));
    }

    // Fill inner loops
    for (int i = 0; i < innerBorders.length(); i++) {
        QVector<QPoint> innerLoop = innerBorders.at(i);
        manipulation->innerloops.append(new FAHLoopInXYPlane());

        for (int j = 0; j < innerLoop.length(); j++) {
            QPoint point = innerLoop.at(j);
            manipulation->innerloops[i]->add(ui2grid(point.x(),point.y(), canvasScale));
        }
    }

    manipulation->location = ui2grid(startingXPos, startingYPos, canvasScale);

    // append Manipulation object to remove it later
    m_Manipulations.append(manipulation);
}

/*!
 * \brief Return modifications in the form suitable for displaying in UI
 * \param manipulationsVector
 * \param canvasSize
 * \return
 */
QVariantList Foot::getModifications(const QVector<Manipulation *> &manipulationsVector,
                                    const QSizeF &canvasSize) const
{
    QVariantList manipulations;
    qreal canvasScale = canvasSize.width() / m_ScanImage.width();
    for (int i = 0; i < manipulationsVector.length(); ++i)
    {
        ManipulationData manipulationData;
        manipulationData.setType(manipulationsVector.at(i)->type);
        QPointF location = grid2ui(manipulationsVector.at(i)->location.x,
                                   manipulationsVector.at(i)->location.y,
                                   canvasScale);
        // location point should be int type
        QPoint point(location.x(), location.y());
        manipulationData.setLocation(point);
        manipulationData.setStiffness(manipulationsVector.at(i)->stiffness);
        manipulationData.setDepth(manipulationsVector.at(i)->depth);
        manipulationData.setThickness(manipulationsVector.at(i)->thickness);
        manipulations.append(QVariant::fromValue(manipulationData));
    }

    return manipulations;
}

/*!
 * \brief Set scan image for foot
 * \param image
 */
void Foot::setScanImage(const QImage &image)
{
    m_ScanImage = image;
}

/*!
 * \brief Return scan foot image to use as background in UI
 */
QImage Foot::getScanImage() const
{
    return m_ScanImage;
}

/*!
 * \brief Return control points for foot
 * \return
 */
QObject *Foot::controlPoints() const
{
    return m_ControlPoints;
}

/*!
 * \brief Set initial fore/heal foot points to calculate
 * initial boundary points
 * \param viewScale
 */
void Foot::calculateInitialPoints(qreal viewScale)
{
    setCanvasScale(viewScale);

    qreal viewWidth = m_ScanImage.width() * m_CanvasScale;
    qreal viewHeight = m_ScanImage.height() * m_CanvasScale;

    // fore points 3/4 of the way up the screen (from the bottom)
    // forming an arch
    qreal foreY = viewHeight / 4;
    qreal foreTopY = foreY - viewHeight / 10;
    qreal fore1X = viewWidth / 12 * 3;
    qreal fore2X = viewWidth / 12 * 5;
    qreal fore3X = viewWidth / 12 * 7;
    qreal fore4X = viewWidth / 12 * 9;
    QVariantList forePoints;
    forePoints.append(QPointF(fore1X, foreY));
    forePoints.append(QPointF(fore2X, foreTopY));
    forePoints.append(QPointF(fore3X, foreTopY));
    forePoints.append(QPointF(fore4X, foreY));

    // heal points 1/4 of the way up the screen (from the bottom)
    // forming an arch
    qreal healY = viewHeight / 4 * 3;
    qreal healBottomY = healY + viewHeight / 10;
    qreal heal1X = viewWidth / 8 * 3;
    qreal heal2X = viewWidth / 8 * 4;
    qreal heal3X = viewWidth / 8 * 5;
    QVariantList healPoints;
    healPoints.append(QPointF(heal1X, healY));
    healPoints.append(QPointF(heal2X, healBottomY));
    healPoints.append(QPointF(heal3X, healY));

    m_ControlPoints->setForePoints(forePoints);
    m_ControlPoints->setHealPoints(healPoints);

    m_ForePoints.clear();
    for (int i = 0; i < forePoints.size(); ++i)
    {
        m_ForePoints.append(ui2grid(forePoints.at(i).toPointF().x(),
                                    forePoints.at(i).toPointF().y()));
    }

    m_HealPoints.clear();
    for (int i = 0; i < healPoints.size(); ++i)
    {
        m_HealPoints.append(ui2grid(healPoints.at(i).toPointF().x(),
                                    healPoints.at(i).toPointF().y()));
    }

    emit forePointsChanged();
    emit healPointsChanged();
}

/*!
 * \brief Return boundary points calculated by backend
 * in the UI coords form (UI can show them without any additional
 * transformations)
 * \param canvasSize - canvas size on which boundary loop will be displayed
 */
QVariantList Foot::getBoundaryPoints(const qreal &canvasScale) const
{
    QVariantList points;
    for (int i = 0; i < m_BoundaryPoints.length(); i += 2)
    {
        points.append(grid2ui(m_BoundaryPoints.at(i),
                              m_BoundaryPoints.at(i + 1),
                              canvasScale));
    }

    return points;
}

/*!
 * \brief Set new boundary points calculated by backend
 * \param points
 */
void Foot::setBoundaryPoints(const QVector<FAHVector3> &points)
{
    m_BoundaryPoints.clear();
    for (int i = 0; i < points.size(); ++i)
    {
        m_BoundaryPoints.append(points.at(i).x);
        m_BoundaryPoints.append(points.at(i).y);
    }

    emit boundaryPointsChanged();
}

/*!
 * \brief Return foot scan width
 * \return
 */
qreal Foot::scanWidth() const
{
    return m_ScanImage.width();
}

/*!
 * \brief Return foot scan height
 * \return
 */
qreal Foot::scanHeight() const
{
    return m_ScanImage.height();
}

/*!
 * \brief Recalculate boundary points by new user fore/heal points
 * \param userPoints - union fore and heal points
 */
void Foot::recalculateBoundary(const QVariantList &userPoints)
{
    QVector<FAHVector3> forePts;

    for (int i = 0; i < m_ForePoints.count(); i++)
    {
        forePts.append(ui2grid(userPoints.at(i).toPointF().x(),
                               userPoints.at(i).toPointF().y()));
    }

    QVector<FAHVector3> healPts;
    for (int i = m_ForePoints.count(); i < userPoints.count(); i++)
    {
        healPts.append(ui2grid(userPoints.at(i).toPointF().x(),
                               userPoints.at(i).toPointF().y()));
    }

    setForePoints(forePts);
    setHealPoints(healPts);
}

/*!
 * \brief Transform point from grid coords to UI view coords
 * \param x grid x coordinate
 * \param y grid y coordinate
 * \see grid2ui(qreal x, qreal y, const qreal &canvasScale)
 */
QPointF Foot::grid2ui(qreal x, qreal y) const
{
    return grid2ui(x, y, m_CanvasScale);
}

/*!
 * \brief Transform point from grid coords to UI view coords
 * \param x grid x coordinate
 * \param y grid y coordinate
 * \param canvasScale ratio of scan image width to view width
 * \return list of view coordinates corresponding to given grid coordinates
 *
 * Operations performed between grid and view (see coordinates.svg)
 * 1. grid is converted to image with coord swap (x,y) -> (y,x)
 * 2. image is flipped cause we have different origin point (x,y) -> (x,H-y),
 * H - scan height, grid origin is in bottom-left corner, view origin is in top-left corner
 * 3. image is mirrored when grid was converted to image (x,y) -> (W-x,y)
 * W - scan width
 * 4. image is stretched on view keeping image aspect ratio (viewScale)
 * (x,y) -> (x*viewScale,y*viewScale)
 */
QPointF Foot::grid2ui(qreal x, qreal y, const qreal &canvasScale) const
{
    qreal uiX = (m_ScanImage.width() - y) * canvasScale;
    qreal uiY = (m_ScanImage.height() - x) * canvasScale;

    QPointF point(uiX, uiY);
    return point;
}

/*!
 * \brief Transform point from UI view coords to grid coords
 * \param x
 * \param y
 * \return
 * \see ui2grid(qreal x, qreal y, const qreal &canvasScale)
 */
FAHVector3 Foot::ui2grid(qreal x, qreal y) const
{
    return ui2grid(x, y, m_CanvasScale);
}

/*!
 * \brief Transform point from UI view coords to grid coords
 * \param x ui x coordinate
 * \param y ui y coordinate
 * \param canvasScale ratio of scan image width to view width
 * \return vector representing point on grid
 * \see grid2ui(qreal x, qreal y, const qreal &canvasScale)
 *
 * This operation is reverse to grid2ui.
 */
FAHVector3 Foot::ui2grid(qreal x, qreal y, const qreal &canvasScale) const
{
    return FAHVector3(m_ScanImage.height() - y / canvasScale,
                      m_ScanImage.width() - x / canvasScale,
                      0);
}

/*!
 * \brief ScanId for foot
 * \param scanId
 */
void Foot::setScanId(const QString &scanId)
{
    m_ScanId = scanId;
}

QString Foot::scanId() const
{
    return m_ScanId;
}

/*!
 * \brief OrthoticId for foot
 * \param orthoticId
 */
void Foot::setOrthoticId(const QString &orthoticId)
{
    m_OrthoticId = orthoticId;
}

QString Foot::orthoticId() const
{
    return m_OrthoticId;
}

/*!
 * \brief Fore point for foot
 * \param forePoints
 */
void Foot::setForePoints(const QVector<FAHVector3> &forePoints)
{
    m_ForePoints = forePoints;

    QVariantList points;
    for (int i = 0; i < m_ForePoints.length(); ++i)
    {
        points.append(grid2ui(m_ForePoints.at(i).x,
                              m_ForePoints.at(i).y));
    }

    m_ControlPoints->setForePoints(points);
}

QVector<FAHVector3> Foot::forePoints() const
{
    return m_ForePoints;
}

/*!
 * \brief heal points for foot
 * \param healPoints
 */
void Foot::setHealPoints(const QVector<FAHVector3> &healPoints)
{
    m_HealPoints = healPoints;

    QVariantList points;
    for (int i = 0; i < m_HealPoints.length(); ++i)
    {
        points.append(grid2ui(m_HealPoints.at(i).x,
                              m_HealPoints.at(i).y));
    }

    m_ControlPoints->setHealPoints(points);
}

QVector<FAHVector3> Foot::healPoints() const
{
    return m_HealPoints;
}

/*!
 * \brief Posting for forefoot
 * \return
 */
Posting *Foot::forePosting() const
{
    return m_ForePosting;
}

/*!
 * \brief Posting for healfoot
 * \return
 */
Posting *Foot::healPosting() const
{
    return m_HealPosting;
}

/*!
 * \brief Foot topcoat
 * \return
 */
Top_Coat *Foot::topCoat() const
{
    return m_TopCoat;
}

/*!
 * \brief Return manipulations vector
 * \return
 */
QVector<Manipulation*> Foot::manipulations() const
{
    return m_Manipulations;
}

/*!
 * \brief Set user changes (posting/topcoat) to Orthotic object
 * \param postingChanges
 * \param topcoatChanges
 */
void Foot::applyUserChanges(const QList<qreal> &postingChanges,
                            const QList<qreal> &topcoatChanges)
{
    if (m_HealPosting == nullptr)
        m_HealPosting = new Posting();
    m_HealPosting->angle = postingChanges.at(0);
    m_HealPosting->verticle = postingChanges.at(1);
    m_HealPosting->varus_valgus = postingChanges.at(2) == 1
            ? Posting::kVargus : Posting::kValgus;
    m_HealPosting->for_rear = Posting::kRearFoot;

    if (m_ForePosting == nullptr)
        m_ForePosting = new Posting();
    m_ForePosting->angle = postingChanges.at(3);
    m_ForePosting->verticle = postingChanges.at(4);
    m_ForePosting->varus_valgus = postingChanges.at(5) == 1
            ? Posting::kVargus : Posting::kValgus;
    m_ForePosting->for_rear = Posting::kForFoot;

    if (m_TopCoat == nullptr)
        m_TopCoat = new Top_Coat();
    m_TopCoat->thickness = topcoatChanges.at(0);
    m_TopCoat->density = static_cast<Top_Coat::Density>(topcoatChanges.at(1));
    m_TopCoat->style = static_cast<Top_Coat::Style>(topcoatChanges.at(2));
}

/*!
 * \brief Return path to STL file that contains whole foot 3D model
 * \return
 */
QString Foot::stlModelFile() const
{
    return m_StlModelFile;
}

void Foot::setStlModelFile(const QString &stlModelFile)
{
    m_StlModelFile = stlModelFile;
}

/*!
 * \brief Return canvas scale
 * \return
 */
qreal Foot::canvasScale() const
{
    return m_CanvasScale;
}

void Foot::setCanvasScale(const qreal &canvasScale)
{
    if(m_CanvasScale != canvasScale) {
        m_CanvasScale = canvasScale;

        emit canvasScaleChanged();
    }
}

/*!
 * \brief Set all settings to default values
 */
void Foot::clean()
{
    m_ForePoints.clear();
    m_HealPoints.clear();
    m_ScanId.clear();
    m_ScanImage = QImage();

    cleanModificationsData();
}

/*!
 * \brief Remove all user modification settings (pads, posting, topcoat)
 */
void Foot::cleanModificationsData()
{
    // free memory and clear manipulations list
    qDeleteAll(m_Manipulations);
    m_Manipulations.clear();

    delete m_ForePosting;
    m_ForePosting = nullptr;
    delete m_HealPosting;
    m_HealPosting = nullptr;
    delete m_TopCoat;
    m_TopCoat = nullptr;
}
