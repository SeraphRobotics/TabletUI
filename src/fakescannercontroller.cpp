#include "fakescannercontroller.h"

#include <QDebug>
#include <QImage>
#include <QSettings>
#include <QTimer>

static const int ProgressMultiplier = 10;

FakeScannerController::FakeScannerController(QObject *parent) :
    QObject(parent),
    m_Counter(0)
{
    QTimer::singleShot(10000, this, SIGNAL(scannerDetected()));

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout,
            [this]()
    {
        m_Counter++;
        if (m_Counter > 10)
        {
            m_timer->stop();
            m_Counter = 0;
            scanComplete();
        }

        emit progressUpdated(m_Counter * ProgressMultiplier);
    });
}

void FakeScannerController::portSelected(QString)
{

}

void FakeScannerController::disconnected()
{
    m_timer->stop();
}

void FakeScannerController::startScan()
{
    buttonPress();
}

void FakeScannerController::buttonPress()
{
    emit scanStarted();
    ScanStep();
}

void FakeScannerController::scannerError()
{
    emit ScanError();
}

void FakeScannerController::scanComplete()
{
    emit ScanComplete();
}

void FakeScannerController::setupCamera()
{

}

void FakeScannerController::ScanStep()
{
    m_Counter = 0;
    m_timer->start();
}

