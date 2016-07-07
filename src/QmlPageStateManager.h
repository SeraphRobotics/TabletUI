#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <QObject>
#include <QString>

class QmlPageStateManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString previousState READ previousState)

public:
    explicit QmlPageStateManager(QObject *parent = 0);

    Q_INVOKABLE void setState(const QString &newState);
    QString previousState() const;

signals:
    void sigSwitchToSpecificScreen(const QString &pageName) const;

private:
    QString m_PreviousState;
    QString m_CurrentState;
};

#endif // STATEMANAGER_H
