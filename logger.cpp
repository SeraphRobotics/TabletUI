#include "logger.h"

#include <QFile>
#include <QMutex>
#include <QCoreApplication>

#include <iostream>

/*!
 * \class Logger
 * \brief Class-singleton that allows to write messages into the file
 * using qDebug(), qWarning(), qInfo(), qCritical(), qFatal()
 */

static const char * const DebugTemplate = "[DEBUG] %1: %2\n";
static const char * const WarningTemplate = "[WARN] %1: %2\n";
static const char * const CriticalTemplate = "[CRITICAL] %1: %2\n";
static const char * const FatalTemplate = "[FATAL] %1: %2\n";
static const char * const InfoTemplate = "[INFO] %1: %2\n";

Logger* Logger::m_instance = nullptr;

void messageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    Q_UNUSED(ctx);
    Logger::instance()->write(type, msg);
}

Logger::Logger() :
    m_mutex(new QMutex()),
    m_logsDirPath(qApp->applicationDirPath()),
    m_logFileNameTemplate("dd_MM_yyyy.log")
{
    qInstallMessageHandler(messageHandler);
}

Logger::~Logger()
{
    if (m_file != nullptr)
    {
        m_file->close();
        delete m_file;
    }

    delete m_mutex;
}

Logger* Logger::instance()
{
    static QMutex mutex;
    if (m_instance == nullptr)
    {
        mutex.lock();
        if (m_instance == nullptr)
            m_instance = new Logger;
        mutex.unlock();
    }
    return m_instance;
}

/*!
 * \brief Write message in accordance with message type
 * \param type
 * \param msg
 */
void Logger::write(QtMsgType type, const QString &msg)
{
    QMutexLocker locker(m_mutex);

    // write message into console too
    std::cout << msg.toUtf8().data() << std::endl;

    if (m_lastCallDate != QDate::currentDate())
    {
        if (m_file != nullptr && m_file->isOpen())
        {
            m_file->close();
            m_file->deleteLater();
        }

        //Create file with date addition
        QString fileName = m_logsDirPath + "/" + QDate::currentDate().toString(m_logFileNameTemplate);
        m_file = new QFile(fileName);
        if (!m_file->open(QIODevice::Append | QIODevice::Text))
            return;

        m_lastCallDate = QDate::currentDate();
    }

    QString datetime = QDateTime::currentDateTime().toString(Qt::ISODate);

    switch (type)
    {
    case QtDebugMsg:
        m_file->write(QString(DebugTemplate).arg(datetime).arg(msg).toUtf8());
        break;
    case QtWarningMsg:
        m_file->write(QString(WarningTemplate).arg(datetime).arg(msg).toUtf8());
        break;
    case QtInfoMsg:
        m_file->write(QString(InfoTemplate).arg(datetime).arg(msg).toUtf8());
        break;
    case QtCriticalMsg:
        m_file->write(QString(CriticalTemplate).arg(datetime).arg(msg).toUtf8());
        break;
    case QtFatalMsg:
        m_file->write(QString(FatalTemplate).arg(datetime).arg(msg).toUtf8());
        abort();
        break;
    default:
        break;
    }
}

/*!
 * \brief Set logs dir path
 * By default, it's set to application directory path
 * \param logsDirPath
 */
void Logger::setLogsDirPath(const QString &logsDirPath)
{
    m_logsDirPath = logsDirPath;

    // remove slashes in the end of path
    while (m_logsDirPath.endsWith("/"))
        m_logsDirPath.chop(1);
}

/*!
 * \brief Set log file template
 * It's used when creating name of the log file.
 * By default, it's set to "dd_MM_yyyy.log"
 * \param logFileNameTemplate
 */
void Logger::setLogFileNameTemplate(const QString &logFileNameTemplate)
{
    m_logFileNameTemplate = logFileNameTemplate;
}

