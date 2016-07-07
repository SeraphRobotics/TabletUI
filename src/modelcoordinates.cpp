#include "modelcoordinates.h"
#include <QtMath>
#include <QtDebug> //TODO remove once not used

/*!
* \class ModelCoordinates
* \brief Provides cooeficient to calclulate between model 3d space coordinates and 2d view coordinates systems
*
* 3D scene coordinates differ from 2D view coordinates system.
* Camera is set in a way to get orthogonal view of xy plane.
* Because of model orientation the we need to use camera up vector (1, 0, 0).
* This result in relations between ui and model coordinates system as presented:
*
* \image scene3d_vs_view_coordinates.png
*
* In order to get boundaries of view for specific z value, camera
* view angle (alpha) and distance for reference point (P) must be taken into account.
* using those values distance between view center and most distant point can be calclulated
* for each axis of plane. Here image represetning x axis:
*
* \image scene3d_camera_range.png
*
* View is centered in (cx, cy) point. To get boundaries maksimum visible range
* for each coordinate must be used.
*
* \image scene3d_bounding_rect.png
*
* To get model coordinates we need to perform linear mapping.
* model = A * ui + B
* model - model coordinate (x or y)
* ui - ui coordinate (x or y)
* A - scale: 2 * dx of model is shown on scene.height, similar for width
* B - view origin (0,0) corresponds to model (cy+dy, cx+dx)
* If scene pos is not (0,0) all ui coordinates must be translated accordingly.
* Also if scene size is smaller then window size (for which framebuffer is created)
* scene will map only fragment of it in 1:1 scale, from bottom-left corner.
* As result x values remain intact  but y values are translated by offset
* eqaul to diffrence between scene and window heights.
*/

ModelCoordinates::ModelCoordinates(QObject *parent, qreal angle, QPointF pos, QSizeF scene,
                                   QSizeF area, QSizeF window, qreal z)
    : QObject(parent)
{
    this->angle = angle;
    this->pos = pos;
    this->scene = scene;
    this->area = area;
    this->window = window;
    pos_z = z;
    updateCoefficients();
}

qreal ModelCoordinates::cameraAngle() const
{
    return angle;
}

qreal ModelCoordinates::cameraRatio() const
{
    return ratio;
}

/*!
 * \brief Distance between far plane and camera
 * \return
 */
qreal ModelCoordinates::cameraHeight() const
{
    return cz - pos_z;
}

const QPointF& ModelCoordinates::cameraShift() const
{
    return shift;
}

QVector3D ModelCoordinates::cameraPosition() const
{
    return QVector3D(cx, cy, cz);
}

QVector3D ModelCoordinates::cameraCenter() const
{
    return QVector3D(cx, cy, 0);
}

const QPointF& ModelCoordinates::scenePos() const
{
    return pos;
}

const QSizeF& ModelCoordinates::sceneSize() const
{
    return scene;
}

const QSizeF& ModelCoordinates::windowSize() const
{
    return window;
}

qreal ModelCoordinates::z() const
{
    return pos_z;
}

void ModelCoordinates::setCameraAngle(qreal angle)
{
    if (this->angle != angle) {
        this->angle = angle;
        coefficientsAreValid = false;
        emit cameraAngleChanged();
    }
}

void ModelCoordinates::setScenePos(QPointF pos)
{
    qDebug() << "scene pos" << pos;
    this->pos = pos;
    coefficientsAreValid = false;
}

void ModelCoordinates::setSceneSize(QSizeF size)
{
    qDebug() << "scene size" << size;
    scene = size;
    coefficientsAreValid = false;
}

void ModelCoordinates::setWindowSize(QSizeF size)
{
    qDebug() << "window size" << size;
    window = size;
    coefficientsAreValid = false;
}

void ModelCoordinates::setAreaSize(QSizeF size)
{
    qDebug() << "area size" << size;
    area = size;
    coefficientsAreValid = false;
}

void ModelCoordinates::setZ(qreal z)
{
    pos_z = z;
    coefficientsAreValid = false;
}

QPointF ModelCoordinates::model2ui(const QPointF& model)
{
    applyChanges();
    return QPointF((model.y() - b2) / b1,
                   (model.x() - a2) / a1);
}

QPointF ModelCoordinates::ui2model(const QPointF& ui)
{
    applyChanges();
    return QPointF(a1 * ui.y() + a2,
                   b1 * ui.x() + b2);

}

void ModelCoordinates::applyChanges()
{
    if(!coefficientsAreValid)
        updateCoefficients();
}

/*!
* \brief calculate params needed to setup view and calclulate coordinates transformation
*
*/
void ModelCoordinates::updateCoefficients()
{
    //Cooeficient for setting 3d camera object properly
    //------------------------------------
    //

    //ratio of window, camera should have the same ratio or frames will be deformed
    ratio = window.width() / window.height();
    //view center z coordinate, counted for dx = max_width
    //To get max_width we need to solve to relation:
    // scene.height  -> area.width
    // window.height -> max_width
    cz = 0.5 * (window.height() / scene.height()) * area.width() / qTan(angle * M_PI / 360) + pos_z;
    //maximum visible distance from view center in x-axis (see scene3d_camera_range.png)
    dx = (cz - pos_z) * qTan(angle * M_PI / 360);
    //maximum visible distance from view center in y-axis (see scene3d_camera_range.png)
    dy = ratio * dx;
    //view center x coordinate
    //we want view to be centered in area center (1/2 * area.width)
    //but if scene size is smaller then window size scene is mapped to bottom-left corner
    //so we need to shift central point to compensate this difference
    //and present in center of scene as expected (possibly Qt3d BUG)
    shift.setX( (window.height() - scene.height()) * dx / window.height() );
    shift.setY( -1 * (window.width() - scene.width()) * dy / window.width() );
    cx = area.width() / 2 + shift.x();
    //view center y coordinate, see comment for cx
    cy = area.height() / 2 + shift.y();

    //Cooeficient for coordinates mapping
    //------------------------------------
    //model.x = a1 * view.y + a2
    //model.y = b1 * view.x + b2
    y_offset = window.height() > scene.height() ? window.height()-scene.height() : 0;
    a1 = -2.0f * dx / window.height();
    a2 = cx + dx + (pos.y() - y_offset) * 2 * dx / window.height();
    b1 = -2.0f * dy / window.width();
    b2 = cy + dy + pos.x() * 2 * dy / window.width();

    coefficientsAreValid = true;
    emit cameraAngleChanged();
    emit cameraRatioChanged();
    emit cameraPositionChanged();
    emit cameraCenterChanged();
    emit cameraShiftChanged();
    emit cameraHeightChanged();
}
