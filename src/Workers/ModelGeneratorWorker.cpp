#include "ModelGeneratorWorker.h"
#include "FileLocationHelper.h"
#include "Models/Foot.h"

#include "Controllers/orthoticscontroller.h"

/*!
 * \class ModelGeneratorWorker
 * \brief Perform generating STL model in separate thread
 */

ModelGeneratorWorker::ModelGeneratorWorker(OrthoticController *orthoticController,
                                           QObject *parent) :
    QObject(parent),
    m_OrthoticController(orthoticController)
{
    connect(m_OrthoticController, &OrthoticController::stlsGenerated,
            this, &ModelGeneratorWorker::processGeneratedStl);
}

/*!
 * \brief Set prefix for genrated STL file
 * \param filePrefix
 */
void ModelGeneratorWorker::setFilePrefix(const QString &filePrefix)
{
    m_FilePrefix = filePrefix;
}

/*!
 * \brief Set foot which should be used in STL generation.
 * All data from foot will be used in STL generation
 * \param foot
 */
void ModelGeneratorWorker::setFoot(Foot *foot)
{
    m_Foot = foot;
}

/*!
 * \brief Set whether worker should save generated STL to file
 * \param saveStlToFile
 */
void ModelGeneratorWorker::setSaveStlToFile(bool saveStlToFile)
{
    m_SaveStlToFile = saveStlToFile;
}

void ModelGeneratorWorker::process()
{
    if (m_Foot != nullptr)
        setFootDataToController(m_OrthoticController, m_Foot);

    m_OrthoticController->makeSTLs();
}

void ModelGeneratorWorker::processGeneratedStl(const QList<View_3D_Item> &items)
{
    if (!m_SaveStlToFile)
    {
        m_OrthoticController->moveToThread(qApp->thread());
        emit modelGenerationFinished();
        return;
    }

    for (int i = 0; i < items.count(); i++) {
        STLFile file;
        file.SetMesh(items.at(i).mesh);
        QString fileName = QString("%1/%2%3.stl").arg(FileLocationHelper::instance()->getStlDir())
                .arg(m_FilePrefix).arg(i);
        file.WriteASCII(fileName);
    }

    m_OrthoticController->moveToThread(qApp->thread());

    emit modelGenerationFinished(QString("%1%2.stl").arg(m_FilePrefix).arg(items.count() - 1));
}

/*!
 * \brief Initialize OrthoticController with foot data
 * \param orthoticController
 * \param foot
 */
void ModelGeneratorWorker::setFootDataToController(OrthoticController *orthoticController,
                                                   Foot *foot)
{
    if (foot->orthoticId().isEmpty())
    {
        orthoticController->setScan(foot->scanId());
        orthoticController->setBorderPoints(foot->healPoints(),
                                              foot->forePoints());
        if (foot->healPosting() != nullptr)
            orthoticController->setPosting(*foot->healPosting());
        if (foot->forePosting() != nullptr)
            orthoticController->setPosting(*foot->forePosting());
        if (foot->topCoat() != nullptr)
            orthoticController->setTopCoat(*foot->topCoat());
        auto manipulations = foot->manipulations();
        for (int i = 0; i < manipulations.size(); ++i)
            orthoticController->addManipulation(manipulations.at(i));
        orthoticController->setBottomType(Orthotic::kCurved);
    }
    else
    {
        orthoticController->setOrthotic(foot->orthoticId());
    }
}

