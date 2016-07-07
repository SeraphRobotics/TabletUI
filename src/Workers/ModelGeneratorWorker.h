#ifndef MODELGENERATORWORKER_H
#define MODELGENERATORWORKER_H

#include <QObject>

#include "View/UI_structs.h"

class OrthoticController;
class Foot;

class ModelGeneratorWorker : public QObject
{
    Q_OBJECT
public:
    explicit ModelGeneratorWorker(OrthoticController *orthoticController,
                                  QObject *parent = 0);

    void process();
    void setFilePrefix(const QString &filePrefix);
    void setFoot(Foot *foot);
    void setSaveStlToFile(bool saveStlToFile);

    static void setFootDataToController(OrthoticController *orthoticController,
                                        Foot *foot);

signals:
    void modelGenerationFinished(const QString &modelFileName = QString()) const;

private:
    void processGeneratedStl(const QList<View_3D_Item> &items);

    OrthoticController *m_OrthoticController;
    QString m_FilePrefix;
    Foot *m_Foot = nullptr;
    bool m_SaveStlToFile = true;
};

#endif // MODELGENERATORWORKER_H
