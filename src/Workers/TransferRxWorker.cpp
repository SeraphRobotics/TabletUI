#include "TransferRxWorker.h"
#include "ModelGeneratorWorker.h"
#include "GcodeGeneratorWorker.h"

#include "Controllers/orthoticscontroller.h"

/*!
 * \class TransferRxWorker
 * \brief Perform transfering Rx to usb flash drive in separate thread
 *
 * First, worker generates STLs for specified Rx, then generates GCode
 * and finally copies generated GCode file to usb flash drive
 */

TransferRxWorker::TransferRxWorker(OrthoticController *orthoticController,
                                   const QString &orthoticId,
                                   const QString &path,
                                   QObject *parent) :
    QObject(parent),
    m_OrthoticController(orthoticController),
    m_OrthoticId(orthoticId),
    m_Path(path)
{
}

void TransferRxWorker::transferRxToUsb()
{
    m_OrthoticController->setOrthotic(m_OrthoticId);
    ModelGeneratorWorker *worker = new ModelGeneratorWorker(m_OrthoticController);
    connect(worker, &ModelGeneratorWorker::modelGenerationFinished,
            this, &TransferRxWorker::processModelGenerationFinished);
    connect(worker, &ModelGeneratorWorker::modelGenerationFinished,
            worker, &ModelGeneratorWorker::deleteLater);

    worker->setSaveStlToFile(false);
    worker->process();
}

void TransferRxWorker::processModelGenerationFinished()
{
    GcodeGeneratorWorker *worker = new GcodeGeneratorWorker(m_OrthoticController, m_Path);
    connect(worker, &GcodeGeneratorWorker::gcodeGenerated,
            this, &TransferRxWorker::transferFinished);
    connect(worker, &GcodeGeneratorWorker::gcodeGenerationFailed,
            this, &TransferRxWorker::transferFailed);
    connect(worker, &GcodeGeneratorWorker::gcodeGenerated,
            worker, &GcodeGeneratorWorker::deleteLater);
    connect(worker, &GcodeGeneratorWorker::gcodeGenerationFailed,
            worker, &GcodeGeneratorWorker::deleteLater);
    worker->generateGcode();
}

