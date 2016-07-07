#ifndef GCODEGENERATORWORKER_H
#define GCODEGENERATORWORKER_H

#include <QObject>

class OrthoticController;

class GcodeGeneratorWorker : public QObject
{
    Q_OBJECT
public:
    explicit GcodeGeneratorWorker(OrthoticController *orthoticController,
                                  const QString &path,
                                  QObject *parent = 0);

    void generateGcode();

private:
    void saveGeneratedGcode(const QString &gcode) const;

    OrthoticController *m_OrthoticController;
    QString m_Path;

signals:
    void gcodeGenerated(const QString fileName) const;
    void gcodeGenerationFailed(const QString &error) const;
};

#endif // GCODEGENERATORWORKER_H
