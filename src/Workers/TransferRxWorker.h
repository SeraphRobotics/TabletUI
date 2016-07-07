#ifndef TRANSFERRXWORKER_H
#define TRANSFERRXWORKER_H

#include <QObject>

class OrthoticController;

class TransferRxWorker : public QObject
{
    Q_OBJECT
public:
    explicit TransferRxWorker(OrthoticController *orthoticController,
                              const QString &orthoticId,
                              const QString &path,
                              QObject *parent = 0);

    void transferRxToUsb();

private:
    void processModelGenerationFinished();

    OrthoticController *m_OrthoticController;
    QString m_OrthoticId;
    QString m_Path;

signals:
    void transferFinished(const QString &fileName) const;
    void transferFailed(const QString &error) const;
};

#endif // TRANSFERRXWORKER_H
