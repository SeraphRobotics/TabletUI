#include "ImageProvider.h"

#include "QmlCppWrapper.h"
#include "Models/Foot.h"

/*!
 * \class ImageProvider
 * \brief Class needed to be able to use QImage objects as the source for Image QML component
 */

ImageProvider::ImageProvider(QmlCppWrapper *wrapper) :
    QQuickImageProvider(QQuickImageProvider::Image),
    m_Wrapper(wrapper)
{
}

/*!
 * \brief Return appropriate foot scan image
 * \param id
 * \param size
 * \param requestedSize
 * \return
 *
 * If foot doesn't contain scan image method return invalid QImage object.
 * If requested size isn't invalid method tries to fit content by width,
 * if computed height more that requested method tries to fit content by height.
 * In any case result image will keep scan grid ratio.
 */
QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Foot *foot = nullptr;
    if (id == "left")
        foot = static_cast<Foot*>(m_Wrapper->leftFoot());
    else if (id == "right")
        foot = static_cast<Foot*>(m_Wrapper->rightFoot());

    if (foot == nullptr || foot->getScanImage().isNull())
        return QImage();

    if (!requestedSize.isValid())
        return foot->getScanImage();

    // fit by width
    int width = requestedSize.width();
    int height = width * foot->scanHeight() / foot->scanWidth();
    if (height > requestedSize.height())
    {
        // height is too big, use fit by height
        height = requestedSize.height();
        width = height * foot->scanWidth() / foot->scanHeight();
    }

    size->setWidth(width);
    size->setHeight(height);
    QImage image = foot->getScanImage();
    return image.scaled(width, height);
}

