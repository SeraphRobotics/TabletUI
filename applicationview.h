#ifndef APPLICATIONVIEW_H
#define APPLICATIONVIEW_H

#include <QObject>
#include <QQuickView>

class ApplicationView : public QQuickView
{
    Q_OBJECT
public:
    ApplicationView();

    Q_PROPERTY(bool capsLockPressed READ capsLockPressed
               NOTIFY sigCapsLockPressedChanged())

    void showExpanded();

public slots :
    Q_INVOKABLE void lockResize();
    Q_INVOKABLE void unlockResize();

    bool capsLockPressed() const;

signals:
    void sigCapsLockPressedChanged() const;

protected:
    virtual void keyReleaseEvent(QKeyEvent * e);

private:
    void _setCapsLockPressed();

private slots:
    void _windowsActiveChanged();

private:
    QSize m_defaultMaximumSize;
    QSize m_normalMinimumSize;

    bool m_CapsLockPressed;
};

#endif // APPLICATIONVIEW_H
