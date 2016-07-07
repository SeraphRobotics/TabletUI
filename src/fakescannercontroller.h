#ifndef FAKESCANNERCONTROLLER_H
#define FAKESCANNERCONTROLLER_H

#include <QObject>
#include <QDir>

class QTimer;

class FakeScannerController : public QObject
{
    Q_OBJECT
public:
    explicit FakeScannerController(QObject *parent = 0);

signals:
    void ScanComplete();
    void ScanError();

    void scanStarted() const;
    void progressUpdated(int progress) const;

    void scannerDetected() const;

public slots:
    void portSelected(QString port);
    void disconnected();
    void startScan();


private slots:
    void buttonPress();
    void scannerError();
    void scanComplete();
    void setupCamera();

    void ScanStep();

private:
    float scandistance_;
    float stepsize_;
    float framerate_;
    int width_;
    int height_;
    float dist;
    QTimer* timer_;
    QDir dir_;

    int m_Counter;
    QTimer *m_timer;
};

#endif // FAKESCANNERCONTROLLER_H
