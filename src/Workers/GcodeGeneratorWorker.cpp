#include "GcodeGeneratorWorker.h"

#include "Controllers/orthoticscontroller.h"
#include "Controllers/printjobcontroller.h"

/*!
 * \class GcodeGeneratorWorker
 * \brief Perform generating GCode in separate thread
 */

GcodeGeneratorWorker::GcodeGeneratorWorker(OrthoticController *orthoticController,
                                           const QString &path,
                                           QObject *parent) :
    QObject(parent),
    m_OrthoticController(orthoticController),
    m_Path(path)
{
}

void GcodeGeneratorWorker::generateGcode()
{
    auto printJobController = new PrintJobController(m_OrthoticController->getOrthotic(), this);
    connect(printJobController, &PrintJobController::GcodeGenerated,
            this, &GcodeGeneratorWorker::saveGeneratedGcode);
    connect(printJobController, &PrintJobController::PrintJobFailed,
            this, &GcodeGeneratorWorker::gcodeGenerationFailed);
    connect(printJobController, &PrintJobController::GcodeGenerated,
            printJobController, &PrintJobController::deleteLater);
    connect(printJobController, &PrintJobController::PrintJobFailed,
            printJobController, &PrintJobController::deleteLater);
    printJobController->RunPrintJob();
}

/*!
 * \brief Perform saving generated GCode into file
 * \param gcode
 */
void GcodeGeneratorWorker::saveGeneratedGcode(const QString &gcode) const
{
    // remove generated gcode in app directory
    // backend always read gcode from this file,
    // so if we have this file and gcode generation was failed
    // backend anyway emit gcodeGenerated signal
    QSettings settings;
    QString dir = settings.value("printing/directory").toString();
    QFile::remove(dir + "/merged.gcode");

    QString fileName = QUuid::createUuid().toString();
    QString filePathTemplate("%1%2.gcode");
    QString filePath = filePathTemplate.arg(m_Path, fileName);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << "Cannot open file" << filePath << "to write GCode";
        emit gcodeGenerationFailed(QStringLiteral("Cannot open file to write GCode"));
        return;
    }

    QTextStream stream(&file);
    stream << gcode;
    file.close();

    emit gcodeGenerated(fileName);
}

