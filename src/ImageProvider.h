#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>

class QmlCppWrapper;

class ImageProvider : public QQuickImageProvider
{
public:
    explicit ImageProvider(QmlCppWrapper *wrapper);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QmlCppWrapper *m_Wrapper;
};

#endif // IMAGEPROVIDER_H
