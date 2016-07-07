#include "QmlCppWrapper.h"
#include "DataStructures/scanmanager.h"
#include "DataStructures/orthoticmanager.h"
#include "DataStructures/usbmanager.h"
#include "Controllers/orthoticscontroller.h"
#include "Controllers/scannerwatcher.h"
#include "Controllers/scannercontroller.h"
#include "Controllers/footrecognizer.h"
#include "Workers/ModelGeneratorWorker.h"
#include "Workers/GcodeGeneratorWorker.h"
#include "Workers/TransferRxWorker.h"
#include "Models/NameObject.h"
#include "Models/UserObject.h"
#include "Models/Foot.h"
#include "Models/UsersListModel.h"
#include "Models/PatientsListModel.h"
#include "Models/PatientDataObject.h"
#include "Models/PatientObject.h"
#include "Models/UsbDataModel.h"
#include "FileLocationHelper.h"
#include "modelcoordinates.h"
#include "logger.h"

#ifdef USE_FAKE_SCANNER
#include "fakescannercontroller.h"
#else
#include "Controllers/scannercontroller.h"
#include "Controllers/scannerwatcher.h"
#endif

#include <QDebug>
#include <QPair>

/*!
 * \class QmlCppWrapper
 * \brief Class used to wrap c++ interface into qml code
 */

static const int PatientNameMinLength = 3;

QmlCppWrapper::QmlCppWrapper(QObject *parent) :
    QObject(parent),
    c_ImagesFolderPath(qApp->applicationDirPath().append("/customPad"))
{
    Logger::instance()->setLogsDirPath(FileLocationHelper::instance()->getLogsDir());
    qDebug() << "QmlCppWrapper ctor";

    m_ScanManager = new ScanManager(this);
    m_OrthoticManager = new OrthoticManager(m_ScanManager, this);
    m_OrthoticController = new OrthoticController(m_OrthoticManager);
    connect(m_OrthoticController, &OrthoticController::scanImageGenerated,
            this, &QmlCppWrapper::processScanImageGenerated);
    connect(m_OrthoticController, &OrthoticController::boundaryLoopUpdated,
            this, &QmlCppWrapper::processUpdatedBoundary);

    m_UsersList = new UsersListModel(this);
    m_PatientsList = new PatientsListModel(this);
    m_UsbDataModel = new UsbDataModel(this);

    m_LeftFoot = new Foot(this);
    m_RightFoot = new Foot(this);

    qRegisterMetaType<NameObject*>("NameObject*");
    qRegisterMetaType<UserObject*>("UserObject*");
    qRegisterMetaType<PatientObject*>("PatientObject*");
    qRegisterMetaType<PatientDataObject*>("PatientDataObject*");
    qRegisterMetaType<QQmlListProperty<UserObject>>("QQmlListProperty<UserObject>");

    setUsersList();
    setPatientsList();

    // create subdirectory for images
    QDir().mkdir(c_ImagesFolderPath);

    connect(m_UsersList, &UsersListModel::sigUsersListReset,
            this, &QmlCppWrapper::setUsersList);

    m_UsbManager = new USBManager(this);
    connect(m_UsbManager, &USBManager::usbConnected,
            [this](const QString &path)
    {
        m_UsbDevices.append(path);

        emit usbDevicesListChanged();
    });

    connect(m_UsbManager, &USBManager::usbDisconnected,
            [this](const QString &path)
    {
        m_UsbDevices.removeAll(path);
        if (m_UsbDevices.isEmpty())
            emit usbDisconnected();

        emit usbDevicesListChanged();
    });

    connect(m_UsbManager, &USBManager::uiUSBItemsUpdated,
            [this](const QList<UI_USB_Item> &usbItems)
    {
        // here we get all scans and rxs on usb disk
        m_UsbDataModel->setItems(usbItems);
        emit usbModelChanged();

        // user plugged in correct flash drive
        if (!usbItems.isEmpty())
            emit usbConnected();
        // user plugged in wrong flash drive
        else if (!m_UsbDevices.isEmpty())
            m_UsbDevices.removeLast();

        emit usbDevicesListChanged();
    });

    m_Preview3dCoordinates = new ModelCoordinates(this, 60, QPointF(0,0), QSizeF(1,1),
                                                  QSizeF(1,1), QSizeF(1,1), 0);
}

QmlCppWrapper::~QmlCppWrapper()
{
    if (m_ScanController != nullptr)
        m_ScanController->disconnected();

    delete m_OrthoticController;
    QDir(c_ImagesFolderPath).removeRecursively();
    delete Logger::instance();
}

/*!
 * \brief set users list from backend
 *
 * Function is called only once in c-tor. Thus, assuming list is empty.
 */
void QmlCppWrapper::setUsersList()
{
    QList<User> users = m_UsersList->getUsersList();

    for (int i = 0; i < users.size(); i++)
    {
        /// @note delete { and } characters from id numbers.
        QRegExp regExp("[{}]");
        QString id = users.at(i).id;
        id.remove(regExp);

        m_UsersList->appendNewUser(new UserObject
                                   (
                                       new NameObject(users.at(i).name.first,
                                                      users.at(i).name.last,
                                                      users.at(i).name.title,
                                                      this),
                                       id,
                                       users.at(i).pwd,
                                       users.at(i).iconfile,
                                       "", //degree
                                       this
                                       )
                                   );
    }
    qDebug() << "Users list size is " << m_UsersList->size();
}

/*!
 * \brief Import Patients list from xml to QtQuick
 */
PatientsListModel* QmlCppWrapper::getPatientsList() const
{
    return m_PatientsList;
}

/*!
 * \brief set patients list from backend
 */
void QmlCppWrapper::setPatientsList()
{
    m_PatientsList->updatePatientsList();
}

/*!
 * \brief Import Users list from xml to QtQuick
 */
UsersListModel* QmlCppWrapper::getUsersList() const
{
    return m_UsersList;
}

/*!
 * \brief current popup showing url Activated via Tutorial button.
 * Or on Page 9 via order materials and contact for support text links.
 */
QString QmlCppWrapper::iFrameUrl() const
{
    return m_iFrameUrl;
}

void QmlCppWrapper::setiFrameUrl(const QString& iFrameUrl)
{
    if(m_iFrameUrl != iFrameUrl)
    {
        m_iFrameUrl = iFrameUrl;
        emit sigiFrameUrlChanged();
    }
}

/*!
 * \brief Returns main application path which is used in qml to save customized or created pads
 * to image folder, more details about this process are in c_ImagesFolderPath description
 */
QString QmlCppWrapper::getApplicationPath() const
{
    return qApp->applicationDirPath();
}

/*!
 * \brief Returns images folder path which is used in qml to save customized or created pads,
 * more details about this process are in c_ImagesFolderPath description
 */
QString QmlCppWrapper::getImagesFolderPath() const
{
    return c_ImagesFolderPath;
}

/*!
 * \brief Get modifications from *.ortho file and
 * convert data for displaying in UI
 * \return
 */
QVariantList QmlCppWrapper::getModifications(const QSizeF &canvasSize) const
{
    // load manipulations data from *.ortho file
    auto orthotic = m_OrthoticController->getOrthotic();
    return m_CurrentFoot->getModifications(orthotic->getManipulations(), canvasSize);
}

/*!
 * \brief Check do we use existing or new *.ortho file.
 * Using this method we can determine
 * should we load modifications/posting/topcoat settings
 * \return
 */
bool QmlCppWrapper::useOrthoFile() const
{
    auto orthotic = m_OrthoticController->getOrthotic();
    if (orthotic == nullptr)
        return false;

    return FileLocationHelper::isOrthoFileExists(orthotic->getId());
}

/*!
 * \brief Return posting and topcoat settings from Orthotic object
 * 0-2 rear posting values
 * 3-5 fore posting values
 * 6-8 topcoat values
 * \return
 */
QVariantList QmlCppWrapper::getOrthoticSettings() const
{
    QVariantList settings;
    auto orthotic = m_OrthoticController->getOrthotic();

    Posting healPosting = orthotic->getRearFootPosting();
    settings.append(healPosting.angle);
    settings.append(healPosting.verticle);
    settings.append(healPosting.varus_valgus);

    Posting forePosting = orthotic->getForFootPosting();
    settings.append(forePosting.angle);
    settings.append(forePosting.verticle);
    settings.append(forePosting.varus_valgus);

    Top_Coat topcoat = orthotic->getTopCoat();
    settings.append(topcoat.thickness);
    settings.append(topcoat.density);
    settings.append(topcoat.style);

    return settings;
}

/*!
 * \brief Qml helper for adding file:/// correctly into image directory.
 * for some odd reason Qt.resolvedUrl() doesn't work here
 */
QString QmlCppWrapper::resolveUrl(const QString &fileName) const
{
    QString name = QUrl::fromLocalFile(fileName).toString();
    qDebug()<<__FUNCTION__<<"Got "<<name;
    return name;
}

/*!
 * \brief Set scanId to OrthoticController and searching for sibling scan id
 * \param scanId
 * \param footType
 */
void QmlCppWrapper::setScan(const QString &scanId, int footType)
{
    QSettings settings;
    QString scanDir = settings.value("scan-directory").toString();
    QString fileNameTemplate("%1/%2.scan");
    if (!QFile::exists(fileNameTemplate.arg(scanDir, scanId)))
    {
        qCritical() << "There is no such scan file at defined scan location:" << scanId;
        emit invalidScan(scanId);
        return;
    }

    cleanFeetData();
    initSiblingFoot(scanId, footType);

    m_OrthoticController->setScan(scanId);
    emit showScan();
}

/*!
 * \brief Set existing *.ortho file and load all the foot data from it
 * If app cannot find such *.ortho file it should use appropriate *.scan file
 * \param orthId
 * \param footType
 */
void QmlCppWrapper::setOrthotic(const QString &orthId, int footType)
{
    if (!m_OrthoticManager->hasOrthotic(orthId)) {
        qCritical() << "There is no Orthotic with id:" << orthId << "in OrthoticManager";
        emit invalidOrthotic(orthId);
        setScan(m_PatientsList->getScanIdByOrthoticId(orthId), footType);
        return;
    }

    QString scanId = m_OrthoticManager->getOrthotic(orthId)->getScanID();
    initSiblingFoot(scanId, footType);

    m_CurrentFoot->setOrthoticId(orthId);
    m_OrthoticController->setOrthotic(orthId);
    // init current foot by values from loaded orthotic
    m_CurrentFoot->setForePoints(m_OrthoticController->getOrthotic()->getForePoints());
    m_CurrentFoot->setHealPoints(m_OrthoticController->getOrthotic()->getHealPoints());
    emit showScan();
}

/*!
 * \brief Determinate current and sibling feet (left or right)
 * and initialized sibling foot
 * \param scanId
 * \param footType
 */
void QmlCppWrapper::initSiblingFoot(const QString &scanId, int footType)
{
    Foot *foot, *siblingFoot;
    if (footType == Foot_Type::kLeft)
    {
        foot = m_LeftFoot;
        siblingFoot = m_RightFoot;
        emit footSideChanged("left");
    }
    else
    {
        foot = m_RightFoot;
        siblingFoot = m_LeftFoot;
        emit footSideChanged("right");
    }

    // get sibling scan id
    QString siblingScanId = m_PatientsList->getSiblingScanIdByScan(scanId);
    QSettings settings;
    QString scanDir = settings.value("scan-directory").toString();
    QString fileNameTemplate("%1/%2.scan");
    if (QFile::exists(fileNameTemplate.arg(scanDir, siblingScanId)))
    {
        siblingFoot->setScanId(siblingScanId);
        m_OrthoticController->setScan(siblingScanId);
        m_SingleFootMode = false;
    }
    else
    {
        qWarning() << "There is no such scan file at defined scan location:" << siblingScanId;
        m_SingleFootMode = true;
    }

    m_CurrentFoot = foot;
    foot->setScanId(scanId);
}

/*!
 * \brief Remove selected *.scan file
 * \param scanId
 */
void QmlCppWrapper::deleteScan(const QString &scanId)
{
    qDebug() << "Delete scan:" << scanId;
    QString siblingScanId = m_PatientsList->getSiblingScanIdByScan(scanId);

    PatientObject* patient = m_PatientsList->getCurrentPatient();
    patient->removeDataObject(scanId, UI_USB_Item::kScan);

    m_PatientsList->removeScanFromPatient(patient->id(), scanId);

    if (FileLocationHelper::isScanFileExists(scanId))
        FileLocationHelper::removeScanFile(scanId);

    if (FileLocationHelper::isScanFileExists(siblingScanId))
        FileLocationHelper::removeScanFile(siblingScanId);
}

/*!
 * \brief Remove selected *.ortho file
 * \param orthId
 */
void QmlCppWrapper::deleteOrthotic(const QString &orthId)
{
    QString siblingOrthId = m_PatientsList->getSiblingOrthoticByOrthoticId(orthId);

    PatientObject* patient = m_PatientsList->getCurrentPatient();
    patient->removeDataObject(orthId, UI_USB_Item::kRx);

    m_PatientsList->removeRxFromPatient(patient->id(), orthId);

    if (FileLocationHelper::isOrthoFileExists(orthId))
        FileLocationHelper::removeOrthoFile(orthId);

    if (FileLocationHelper::isOrthoFileExists(siblingOrthId))
        FileLocationHelper::removeOrthoFile(siblingOrthId);

    m_PatientsList->updatePatientsList();
}

/*!
 * \brief Initialize fore/heal points for both feet to calculate initial boundary points
 * \param leftViewScale
 * \param rightViewScale
 */
void QmlCppWrapper::initFeet(qreal leftViewScale, qreal rightViewScale)
{
    // first initialize non-current foot to leave correct current foot
    if (m_CurrentFoot == m_RightFoot)
    {
        if (!m_LeftFoot->scanId().isEmpty())
        {
            m_CurrentFoot = m_LeftFoot;
            m_LeftFoot->calculateInitialPoints(leftViewScale);
            m_OrthoticController->setBorderPoints(m_LeftFoot->healPoints(),
                                                  m_LeftFoot->forePoints());
            m_CurrentFoot = m_RightFoot;
        }

        m_RightFoot->calculateInitialPoints(rightViewScale);
        m_OrthoticController->setBottomType(Orthotic::kCurved);
        m_OrthoticController->setBorderPoints(m_RightFoot->healPoints(),
                                              m_RightFoot->forePoints());
    }
    else if (m_CurrentFoot == m_LeftFoot)
    {
        if (!m_RightFoot->scanId().isEmpty())
        {
            m_CurrentFoot = m_RightFoot;
            m_RightFoot->calculateInitialPoints(rightViewScale);
            m_OrthoticController->setBorderPoints(m_RightFoot->healPoints(),
                                                  m_RightFoot->forePoints());
            m_CurrentFoot = m_LeftFoot;
        }

        m_LeftFoot->calculateInitialPoints(leftViewScale);
        m_OrthoticController->setBottomType(Orthotic::kCurved);
        m_OrthoticController->setBorderPoints(m_LeftFoot->healPoints(),
                                              m_LeftFoot->forePoints());
    }
}

/*!
 * \brief Perform flip image and save its width and height scalings
 * \param image
 */
void QmlCppWrapper::processScanImageGenerated(const QImage &image)
{
    auto scan = m_OrthoticController->getOrthotic()->getScan();
    auto grid = scan->getProcessedXYGrid();
    // if scan hasn't postedGrid we set processedGrid as postedGrid
    if (scan->getPostedXYGrid() == nullptr)
        scan->setPostedGrid(grid);

    //We need to flip image because it is computed for origin point
    //at (left,bottom) and we gonna to show in default screen coordinates
    //with origin point at (left,top)
    QTransform flipTransform;
    flipTransform.rotate(180);
    QImage transformedImage = image.transformed(flipTransform);

    // coordinates were reordered when image was generated
    // Ximage = Ygrid, Yimage = Xgrid
    // apply grid step scaling to image
    transformedImage = transformedImage.scaled(transformedImage.width() * grid->stepSizeY(),
                                               transformedImage.height() * grid->stepSizeX());

    if (m_OrthoticController->getOrthotic()->getScanID() == m_LeftFoot->scanId())
    {
        m_LeftFoot->setScanImage(transformedImage);
        if (m_CurrentFoot == m_LeftFoot)
            emit scanImageGenerated();
    }
    else if (m_OrthoticController->getOrthotic()->getScanID() == m_RightFoot->scanId())
    {
        m_RightFoot->setScanImage(transformedImage);
        if (m_CurrentFoot == m_RightFoot)
            emit scanImageGenerated();
    }
}

/*!
 * \brief Save new calculated loop by backend as is
 * \param loop
 */
void QmlCppWrapper::processUpdatedBoundary(FAHLoopInXYPlane *loop)
{
    m_CurrentFoot->setBoundaryPoints(loop->points);

    auto orthotic = m_OrthoticController->getOrthotic();
    m_CurrentFoot->setForePoints(orthotic->getForePoints());
    m_CurrentFoot->setHealPoints(orthotic->getHealPoints());

    emit boundaryUpdated();
}

/*!
 * \brief Recalculate boundary points by new user fore/heal points
 * \param userPoints - union fore and heal points
 * \param canvasScale - scaling between grid and UI values
 */
void QmlCppWrapper::recalculateBoundary(const QVariantList &userPoints, qreal canvasScale)
{
    m_CurrentFoot->setCanvasScale(canvasScale);
    m_CurrentFoot->recalculateBoundary(userPoints);

    m_OrthoticController->setBorderPoints(m_CurrentFoot->healPoints(),
                                          m_CurrentFoot->forePoints());
}

/*!
 * \brief Save *.ortho file
 * 1) if this *.ortho file exists we just rewrite it
 * 2) if this is new *.ortho we create it and add rx info to patient (patients.xml)
 * \param patientId
 * \param doctorId
 */
void QmlCppWrapper::saveOrtho(const QString &patientId, const QString &doctorId)
{
    disconnectOrthoticController();
    ModelGeneratorWorker::setFootDataToController(m_OrthoticController, m_LeftFoot);
    auto leftOrthotic = m_OrthoticController->getOrthotic();
    leftOrthotic->setFootType(Orthotic::kLeft);
    QString leftOrthoticId = leftOrthotic->getId();
    bool createNewRx = !FileLocationHelper::isOrthoFileExists(leftOrthoticId);

    m_OrthoticController->save();

    ModelGeneratorWorker::setFootDataToController(m_OrthoticController, m_RightFoot);
    auto rightOrthotic = m_OrthoticController->getOrthotic();
    rightOrthotic->setFootType(Orthotic::kRight);
    QString rightOrthoticId = rightOrthotic->getId();
    createNewRx = createNewRx || !FileLocationHelper::isOrthoFileExists(rightOrthoticId);

    m_OrthoticController->save();

    if (createNewRx)
    {
        Rx rx;
        rx.docId = doctorId;
        rx.date = QDate::currentDate();
        rx.note = "";
        rx.shoesize = 9.0;
        rx.leftScanId = leftOrthotic->getScanID();
        rx.rightScanId = rightOrthotic->getScanID();
        rx.leftOrthoticId = leftOrthoticId;
        rx.rightOrthoticId = rightOrthoticId;
        rx.orthoticAvailable = !leftOrthoticId.isEmpty() || !rightOrthoticId.isEmpty();
        m_PatientsList->replaceRxAtPatient(patientId, leftOrthotic->getScanID(), rx);

        m_PatientsList->updatePatientsList();
    }

    connectOrthoticController();
}

/*!
 * \brief Request generating STL models for both feet
 * in accordance with current boundary points
 */
void QmlCppWrapper::generateStlModels()
{
#ifdef USE_EXISTING_STL
    auto helper = FileLocationHelper::instance();
    if (QFile::exists(helper->getStlDir() + "/left0.stl")
            && QFile::exists(helper->getStlDir() + "/right0.stl"))
    {
        m_LeftFoot->setStlModelFile(helper->getStlUrlPath("left0.stl"));
        m_RightFoot->setStlModelFile(helper->getStlUrlPath("right0.stl"));
        QMetaObject::invokeMethod(this,
                                  "hideAnimationProgress",
                                  Qt::QueuedConnection);
        return;
    }
#endif

    emit showAnimationProgress(tr("Generating STL models..."));
    disconnectOrthoticController();
    generateStlModel(m_CurrentFoot);
}

/*!
 * \brief Generate STL model(s) for foot in separate thread
 * \param foot
 */
void QmlCppWrapper::generateStlModel(Foot *foot)
{
    QThread *workerThread = new QThread(this);
    m_OrthoticController->moveToThread(workerThread);
    ModelGeneratorWorker *worker = new ModelGeneratorWorker(m_OrthoticController);

    connect(workerThread, &QThread::finished,
            worker, &ModelGeneratorWorker::deleteLater);
    connect(workerThread, &QThread::finished,
            workerThread, &QThread::deleteLater);
    connect(workerThread, &QThread::started,
            worker, &ModelGeneratorWorker::process);
    connect(worker, &ModelGeneratorWorker::modelGenerationFinished,
            workerThread, &QThread::quit);
    connect(worker, &ModelGeneratorWorker::modelGenerationFinished,
            this, &QmlCppWrapper::processModelGenerationFinished);

    worker->moveToThread(workerThread);
    worker->setFilePrefix(foot == m_LeftFoot ? "left" : "right");
    worker->setFoot(foot);
    workerThread->start();
}

/*!
 * \brief Disconnect OrthoticController signals
 * to set foot data before generating STL models
 */
void QmlCppWrapper::disconnectOrthoticController()
{
    disconnect(m_OrthoticController, &OrthoticController::scanImageGenerated,
               this, &QmlCppWrapper::processScanImageGenerated);
    disconnect(m_OrthoticController, &OrthoticController::boundaryLoopUpdated,
               this, &QmlCppWrapper::processUpdatedBoundary);
}

/*!
 * \brief Restore OrthoticController signals which were disconnected
 * before generating STL models
 */
void QmlCppWrapper::connectOrthoticController()
{
    connect(m_OrthoticController, &OrthoticController::scanImageGenerated,
            this, &QmlCppWrapper::processScanImageGenerated);
    connect(m_OrthoticController, &OrthoticController::boundaryLoopUpdated,
            this, &QmlCppWrapper::processUpdatedBoundary);
}

/*!
 * \brief Save generated file name for appropriate foot
 * and start generation for another foot or finish generation process
 * \param modelFileName
 */
void QmlCppWrapper::processModelGenerationFinished(const QString &modelFileName)
{
    auto helper = FileLocationHelper::instance();
    auto anotherFoot = m_CurrentFoot == m_LeftFoot ? m_RightFoot : m_LeftFoot;
    if (m_OrthoticController->getOrthotic()->getScanID() == m_CurrentFoot->scanId())
    {
        m_CurrentFoot->setStlModelFile(helper->getStlUrlPath(modelFileName));
        if (anotherFoot->scanId().isEmpty())
        {
            connectOrthoticController();
            emit hideAnimationProgress();
        }
        else
        {
            generateStlModel(anotherFoot);
        }
    }
    else
    {
        anotherFoot->setStlModelFile(helper->getStlUrlPath(modelFileName));
        ModelGeneratorWorker::setFootDataToController(m_OrthoticController, m_CurrentFoot);
        connectOrthoticController();
        emit hideAnimationProgress();
    }
}

/*!
 * \brief Request GCode generation by backend in separate thread
 */
void QmlCppWrapper::generateGcode()
{
    emit showAnimationProgress(tr("Generating GCode..."));
    QThread *workerThread = new QThread(this);
    GcodeGeneratorWorker *worker = new GcodeGeneratorWorker(m_OrthoticController,
                                                            m_UsbDevices.last());

    connect(workerThread, &QThread::finished,
            worker, &GcodeGeneratorWorker::deleteLater);
    connect(workerThread, &QThread::finished,
            workerThread, &QThread::deleteLater);
    connect(workerThread, &QThread::started,
            worker, &GcodeGeneratorWorker::generateGcode);
    connect(worker, &GcodeGeneratorWorker::gcodeGenerated,
            workerThread, &QThread::quit);
    connect(worker, &GcodeGeneratorWorker::gcodeGenerationFailed,
            workerThread, &QThread::quit);
    connect(worker, &GcodeGeneratorWorker::gcodeGenerationFailed,
            this, &QmlCppWrapper::generateGcodeFailed);
    connect(worker, &GcodeGeneratorWorker::gcodeGenerated,
            this, &QmlCppWrapper::processGcodeGenerated);

    worker->moveToThread(workerThread);
    workerThread->start();
}

/*!
 * \brief Process success gcode generation
 * \param fileName
 */
void QmlCppWrapper::processGcodeGenerated(const QString &fileName)
{
    addRxItem(fileName);
    emit generateGcodeFinished();
}

/*!
 * \brief Start scanner work
 */
void QmlCppWrapper::startScanner()
{
    m_ScanController->startScan();
}

/*!
 * \brief Return is usb was detected
 * \return
 */
bool QmlCppWrapper::isUsbDetected() const
{
    return !m_UsbDevices.isEmpty();
}

/*!
 * \brief Sort scans according to the rule: first is left foot scan,
 * second is right foot scan
 * \param scanIds
 * \return sorted pair
 */
QPair<QString, QString> QmlCppWrapper::getLeftRightScans(const QStringList &scanIds) const
{
    QPair<QString, QString> leftRightScans;
    foreach (const QString &scanId, scanIds)
    {
        if (scanId.isEmpty())
            continue;

        auto footType = FootRecognizer::recognize(
                    FileLocationHelper::instance()->getScanFullPath(scanId));
        if (footType == FootRecognizer::LeftFoot)
            leftRightScans.first = scanId;
        else
            leftRightScans.second = scanId;
    }

    return leftRightScans;
}

/*!
 * \brief Perform adding scans list to existing patient
 * \param scanIds
 * \param patientId
 * \param doctorId
 */
void QmlCppWrapper::addScanToExistingPatient(const QStringList &scanIds,
                                             const QString &patientId,
                                             const QString &doctorId)
{
    foreach (const QString &scanId, scanIds)
        copyScanFromUsb(scanId);

    m_OrthoticManager->sm_->updateScanList();
    auto sortedPair = getLeftRightScans(scanIds);
    m_PatientsList->addScansToPatient(sortedPair.first,
                                      sortedPair.second,
                                      patientId,
                                      doctorId);
    setPatientsList();
}

/*!
 * \brief Perform adding scans list to new patient
 * \param scanIds
 * \param patientName
 * \param comments
 * \param doctorId
 */
void QmlCppWrapper::newPatientFromScan(const QStringList &scanIds,
                                       const QString &patientName,
                                       const QString &comments,
                                       const QString &doctorId)
{
    foreach (const QString &scanId, scanIds)
        copyScanFromUsb(scanId);

    m_OrthoticManager->sm_->updateScanList();
    auto sortedPair = getLeftRightScans(scanIds);
    m_PatientsList->newPatientFromScans(sortedPair.first,
                                        sortedPair.second,
                                        patientName,
                                        comments,
                                        doctorId);
    setPatientsList();
}

/*!
 * \brief Copy specified scan file from usb flash drive
 * \param scanId
 */
void QmlCppWrapper::copyScanFromUsb(const QString &scanId) const
{
    if (scanId.isEmpty())
        return;

    if (!FileLocationHelper::isScanFileExists(scanId))
    {
        QString scanPath = m_UsbDevices.last() + scanId + ".scan";
        QString newScanPath = FileLocationHelper::instance()->getScanFullPath(scanId);
        QFile::copy(scanPath, newScanPath);
    }
}

/*!
 * \brief Left patient foot
 * \param leftFoot
 */
void QmlCppWrapper::setLeftFoot(QObject *leftFoot)
{
    if (m_LeftFoot != leftFoot) {
        m_LeftFoot = static_cast<Foot*>(leftFoot);

        emit leftFootChanged();
    }
}

QObject *QmlCppWrapper::leftFoot() const
{
    return m_LeftFoot;
}

/*!
 * \brief Right patient foot
 * \param leftFoot
 */
void QmlCppWrapper::setRightFoot(QObject *rightFoot)
{
    if (m_RightFoot != rightFoot) {
        m_RightFoot = static_cast<Foot*>(rightFoot);

        emit rightFootChanged();
    }
}

QObject *QmlCppWrapper::rightFoot() const
{
    return m_RightFoot;
}

/*!
 * \brief Current patient foot (left or right)
 * \param foot
 */
void QmlCppWrapper::setCurrentFoot(QObject *foot)
{
    if (m_CurrentFoot != foot) {
        m_CurrentFoot = static_cast<Foot*>(foot);

        emit currentFootChanged();
    }
}

QObject *QmlCppWrapper::currentFoot() const
{
    return m_CurrentFoot;
}

/*!
 * \brief Final 3d preview3d coordinates helper object
 * \return model coordinates
 *
 * This class help to set camera properly despite Scene3d buggy behaviour.
 */
QObject* QmlCppWrapper::preview3dCoordinates() const
{
    return m_Preview3dCoordinates;
}

/*!
 * \brief Return usb filesystem model
 * \return
 */
QObject *QmlCppWrapper::usbModel() const
{
    return m_UsbDataModel;
}

/*!
 * \brief Remove file from usb filesystem
 * \param index
 */
void QmlCppWrapper::removeUsbItem(int index)
{
    QString id = m_UsbDataModel->removeItem(index);
    m_UsbManager->deleteItem(id);
}

/*!
 * \brief Perform transfering Rx to usb flash drive
 * \param orthoId
 *
 * App generates STL models and GCode for specified Rx and save result to usb.
 * All processes are performed in separate thread.
 */
void QmlCppWrapper::transferRxToUsb(const QString &orthoId)
{
    if (m_UsbDevices.isEmpty())
    {
        emit transferRxFailed(QStringLiteral("No usb detected"));
        return;
    }

    if (!m_OrthoticManager->hasOrthotic(orthoId))
    {
        emit transferRxFailed(QStringLiteral("Could not load Orthotic with id: %1").arg(orthoId));
        return;
    }

    emit showAnimationProgress(tr("Transfering Rx to USB drive..."));
    disconnectOrthoticController();
    QThread *workerThread = new QThread(this);
    m_OrthoticController->moveToThread(workerThread);
    TransferRxWorker *worker = new TransferRxWorker(m_OrthoticController,
                                                    orthoId,
                                                    m_UsbDevices.last());
    connect(workerThread, &QThread::finished,
            worker, &TransferRxWorker::deleteLater);
    connect(workerThread, &QThread::finished,
            workerThread, &QThread::deleteLater);
    connect(workerThread, &QThread::started,
            worker, &TransferRxWorker::transferRxToUsb);
    connect(worker, &TransferRxWorker::transferFinished,
            workerThread, &QThread::quit);
    connect(worker, &TransferRxWorker::transferFailed,
            workerThread, &QThread::quit);
    connect(worker, &TransferRxWorker::transferFailed,
            this, &QmlCppWrapper::transferRxFailed);
    connect(worker, &TransferRxWorker::transferFinished,
            this, &QmlCppWrapper::processTransferRxFinished);

    worker->moveToThread(workerThread);
    workerThread->start();
}

/*!
 * \brief Process successfully completed transfer Rx to Usb
 * \param fileName
 */
void QmlCppWrapper::processTransferRxFinished(const QString &fileName)
{
    addRxItem(fileName);
    emit transferRxFinished();
}

/*!
 * \brief Add new Rx item to patient (patients.xml)
 * \param fileName
 */
void QmlCppWrapper::addRxItem(const QString &fileName)
{
    UI_USB_Item item;
    item.datetime = QDateTime::currentDateTime();
    item.id = fileName;
    item.type = UI_USB_Item::kRx;
    NameObject *name = m_PatientsList->getCurrentPatient()->name();
    item.patient = Name(name->title(), name->firstName(), name->lastName());
    m_UsbManager->addItem(item);
    m_UsbDataModel->addItem(item);
}

/*!
 * \brief Set all foot settings to default values
 * for left/right foot and OrthoticController
 */
void QmlCppWrapper::cleanFeetData()
{
    disconnectOrthoticController();
    m_OrthoticController->clean();
    m_LeftFoot->clean();
    m_RightFoot->clean();
    connectOrthoticController();
}

/*!
 * \brief Remove all user modification settings (pads, posting, topcoat)
 * for left/right foot and OrthoticController
 */
void QmlCppWrapper::cleanFeetModifications()
{
    disconnectOrthoticController();
    m_OrthoticController->clean();
    m_LeftFoot->cleanModificationsData();
    m_RightFoot->cleanModificationsData();
    connectOrthoticController();
}

/*!
 * \brief Return whether app uses only 1 foot scan per Rx (single foot mode)
 * \return
 *
 * It depends on patient's scan files count (one foot or two feet in Rx)
 */
bool QmlCppWrapper::isSingleFootMode() const
{
    return m_SingleFootMode;
}

/*!
 * \brief Perform scanner initialization when user logged in
 */
void QmlCppWrapper::initScanner()
{
    if (m_ScanController != nullptr)
        delete m_ScanController;

#ifdef USE_FAKE_SCANNER
    m_ScanController = new FakeScannerController(this);
    connect(m_ScanController, &FakeScannerController::scannerDetected,
            this, &QmlCppWrapper::scannerDetected);
    connect(m_ScanController, &FakeScannerController::progressUpdated,
            this, &QmlCppWrapper::scanProgressUpdated);
    connect(m_ScanController, &FakeScannerController::ScanComplete,
            this, &QmlCppWrapper::scanCompleted);
    connect(m_ScanController, &FakeScannerController::ScanError,
            this, &QmlCppWrapper::scanError);
    connect(m_ScanController, &FakeScannerController::scanStarted,
            this, &QmlCppWrapper::scanStarted);
#else
    m_ScanController = new ScannerController(this);
    ScannerWatcher *scannerWatcher = new ScannerWatcher(this);
    connect(scannerWatcher, &ScannerWatcher::scannerPort,
            [this](const QString &port)
    {
        m_ScanController->portSelected(port);
        emit scannerDetected();
    });
    connect(scannerWatcher, &ScannerWatcher::disconnected,
            [this]()
    {
        m_ScanController->disconnected();
        emit scannerDisconnected();
    });
    connect(m_ScanController, &ScannerController::progressUpdated,
            this, &QmlCppWrapper::scanProgressUpdated);
    connect(m_ScanController, &ScannerController::ScanComplete,
            this, &QmlCppWrapper::scanCompleted);
    connect(m_ScanController, &ScannerController::ScanError,
            this, &QmlCppWrapper::scanError);
    connect(m_ScanController, &ScannerController::scanStarted,
            this, &QmlCppWrapper::scanStarted);
#endif
}

/*!
 * \brief Perform cleaning scanner device operations
 */
void QmlCppWrapper::cleanScanner()
{
    m_ScanController->disconnected();
}

/*!
 * \brief Return minimum length of the patient name
 * \return
 */
int QmlCppWrapper::patientNameMinLength() const
{
    return PatientNameMinLength;
}
