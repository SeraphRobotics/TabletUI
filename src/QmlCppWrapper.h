#ifndef QMLCPPWRAPPER_H
#define QMLCPPWRAPPER_H

#include <QObject>
#include <QVariant>

#include "View/UI_structs.h"

class ScanManager;
class OrthoticManager;
class OrthoticController;
class Foot;
class ModelCoordinates;
class UsersListModel;
class PatientsListModel;
class UsbDataModel;
class USBManager;

#ifdef USE_FAKE_SCANNER
class FakeScannerController;
#else
class ScannerController;
#endif

class QmlCppWrapper : public QObject
{
    Q_OBJECT

    /// @note Take a look at m_iFrameUrl description.
    Q_PROPERTY(QString iFrameUrl READ iFrameUrl WRITE setiFrameUrl
               NOTIFY sigiFrameUrlChanged)
    Q_PROPERTY(bool usbDetected READ isUsbDetected NOTIFY usbDevicesListChanged)
    Q_PROPERTY(QObject *leftFoot READ leftFoot WRITE setLeftFoot NOTIFY leftFootChanged)
    Q_PROPERTY(QObject *rightFoot READ rightFoot WRITE setRightFoot NOTIFY rightFootChanged)
    Q_PROPERTY(QObject *currentFoot READ currentFoot WRITE setCurrentFoot NOTIFY currentFootChanged)
    Q_PROPERTY(QObject *preview3d READ preview3dCoordinates CONSTANT)
    Q_PROPERTY(QObject *usbModel READ usbModel NOTIFY usbModelChanged)
    Q_PROPERTY(int patientNameMinLength READ patientNameMinLength CONSTANT)

public:
    explicit QmlCppWrapper(QObject *parent = 0);
    ~QmlCppWrapper();

public:
    UsersListModel *getUsersList() const;
    PatientsListModel *getPatientsList() const;
    QString iFrameUrl() const;
    void setiFrameUrl(const QString& iFrameUrl);
    bool isUsbDetected() const;

    Q_INVOKABLE QString getApplicationPath() const;
    Q_INVOKABLE QString getImagesFolderPath() const;
    Q_INVOKABLE QString resolveUrl(const QString &fileName) const;
    Q_INVOKABLE QVariantList getModifications(const QSizeF &canvasSize) const;
    Q_INVOKABLE QVariantList getOrthoticSettings() const;
    Q_INVOKABLE bool useOrthoFile() const;
    Q_INVOKABLE void setScan(const QString &scanId, int footType);
    Q_INVOKABLE void setOrthotic(const QString &orthId, int footType);
    Q_INVOKABLE void deleteScan(const QString &scanId);
    Q_INVOKABLE void deleteOrthotic(const QString &orthId);
    Q_INVOKABLE void saveOrtho(const QString &patientId, const QString &doctorId);
    Q_INVOKABLE void generateStlModels();
    Q_INVOKABLE void generateGcode();
    Q_INVOKABLE void recalculateBoundary(const QVariantList &userPoints, qreal canvasScale);
    Q_INVOKABLE void initFeet(qreal leftViewScale, qreal rightViewScale);
    Q_INVOKABLE void startScanner();
    Q_INVOKABLE void addScanToExistingPatient(const QStringList &scanIds,
                                              const QString &patientId,
                                              const QString &doctorId);
    Q_INVOKABLE void newPatientFromScan(const QStringList &scanIds,
                                        const QString &patientName,
                                        const QString &comments,
                                        const QString &doctorId);
    Q_INVOKABLE void removeUsbItem(int index);
    Q_INVOKABLE void transferRxToUsb(const QString &orthoId);
    Q_INVOKABLE void cleanFeetModifications();
    Q_INVOKABLE bool isSingleFootMode() const;
    Q_INVOKABLE void initScanner();
    Q_INVOKABLE void cleanScanner();

    void setLeftFoot(QObject *leftFoot);
    QObject *leftFoot() const;
    void setRightFoot(QObject *rightFoot);
    QObject *rightFoot() const;
    void setCurrentFoot(QObject *foot);
    QObject *currentFoot() const;
    QObject *preview3dCoordinates() const;
    QObject *usbModel() const;
    int patientNameMinLength() const;

signals:
    void sigiFrameUrlChanged() const;
    void showAnimationProgress(const QString &text) const;
    void hideAnimationProgress() const;
    void generateGcodeFinished() const;
    void generateGcodeFailed(const QString &error) const;
    void usbConnected() const;
    void usbDisconnected() const;
    void usbDevicesListChanged() const;
    void scannerDetected() const;
    void scannerDisconnected() const;
    void scanProgressUpdated(int progress) const;
    void scanCompleted() const;
    void scanError() const;
    void scanStarted() const;
    void scanImageGenerated() const;
    void boundaryUpdated() const;
    void invalidOrthotic(const QString &orthId) const;
    void leftFootChanged() const;
    void rightFootChanged() const;
    void currentFootChanged() const;
    void usbModelChanged() const;
    void invalidScan(const QString &scanId) const;
    void showScan() const;
    void transferRxFinished() const;
    void transferRxFailed(const QString &error) const;
    void footSideChanged(const QString &footSide) const;

private:
    void setUsersList();
    void setPatientsList();
    void processUpdatedBoundary(FAHLoopInXYPlane *loop);
    void processScanImageGenerated(const QImage &image);
    void processModelGenerationFinished(const QString &modelFileName);
    void processGcodeGenerated(const QString &fileName);
    void processTransferRxFinished(const QString &fileName);
    void generateStlModel(Foot *foot);
    void disconnectOrthoticController();
    void connectOrthoticController();
    QPair<QString, QString> getLeftRightScans(const QStringList &scanIds) const;
    void addRxItem(const QString &fileName);
    void cleanFeetData();
    void initSiblingFoot(const QString &scanId, int footType);
    void copyScanFromUsb(const QString &scanId) const;

    ScanManager *m_ScanManager = nullptr;
    OrthoticManager *m_OrthoticManager = nullptr;
    OrthoticController *m_OrthoticController = nullptr;
#ifdef USE_FAKE_SCANNER
    FakeScannerController *m_ScanController = nullptr;
#else
    ScannerController *m_ScanController = nullptr;
#endif
    QVector<QString> m_UsbDevices;
    UsersListModel *m_UsersList = nullptr;
    PatientsListModel *m_PatientsList = nullptr;
    QString m_iFrameUrl;
    const QString c_ImagesFolderPath;
    /// @note path for folder where application saving all custom created pads images.
    /// Currently application just save it on application working and delete or application
    ///  This way we can easily add all saved pad always on application start.If we want.

    Foot *m_LeftFoot = nullptr;
    Foot *m_RightFoot = nullptr;
    Foot *m_CurrentFoot = nullptr;
    ModelCoordinates *m_Preview3dCoordinates = nullptr;
    UsbDataModel *m_UsbDataModel = nullptr;
    USBManager *m_UsbManager = nullptr;
    bool m_SingleFootMode = false;
};

#endif // QMLCPPWRAPPER_H
