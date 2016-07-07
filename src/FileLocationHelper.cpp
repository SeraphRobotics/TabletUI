#include "FileLocationHelper.h"

#include <QStandardPaths>
#include <QDir>
#include <QMutex>
#include <QGuiApplication>

/*!
 * \class FileLocationHelper
 * \brief Helper singleton class for managing files location
 */

static const char * const ScanDirName = "scan";
static const char * const OrthoDirName = "ortho";
static const char * const StlDirName = "stl";
static const char * const LogsDirName = "logs";

FileLocationHelper *FileLocationHelper::m_Instance = nullptr;

/*!
 * \brief Create new directory if it doesn't exist
 * \param dirPath
 */
void createDirIfNotExists(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists())
    {
        bool ok = dir.mkpath(dirPath);
        Q_ASSERT(ok);
    }
}

FileLocationHelper::FileLocationHelper()
{
#ifdef Q_OS_WIN
    QString rootDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
    QString rootDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
            + QDir::separator() + qApp->applicationName();
#endif
    createDirIfNotExists(rootDir);

    m_ScanDirPath = rootDir + QDir::separator() + ScanDirName;
    createDirIfNotExists(m_ScanDirPath);

    m_OrthoDirPath = rootDir + QDir::separator() + OrthoDirName;
    createDirIfNotExists(m_OrthoDirPath);

    m_StlDirPath = rootDir + QDir::separator() + StlDirName;
    createDirIfNotExists(m_StlDirPath);

    m_LogsDirPath = rootDir + QDir::separator() + LogsDirName;
    createDirIfNotExists(m_LogsDirPath);
}

FileLocationHelper *FileLocationHelper::instance()
{
    static QMutex mutex;
    if (m_Instance == nullptr)
    {
        mutex.lock();
        if (m_Instance == nullptr)
            m_Instance = new FileLocationHelper;
        mutex.unlock();
    }
    return m_Instance;
}

/*!
 * \brief Return scans dir full path
 * \return
 */
QString FileLocationHelper::getScanDir() const
{
    return m_ScanDirPath;
}

/*!
 * \brief Return orthotics dir full path
 * \return
 */
QString FileLocationHelper::getOrthoDir() const
{
    return m_OrthoDirPath;
}

/*!
 * \brief Return stls dir full path
 * \return
 */
QString FileLocationHelper::getStlDir() const
{
    return m_StlDirPath;
}

/*!
 * \brief Return logs dir full path
 * \return
 */
QString FileLocationHelper::getLogsDir() const
{
    return m_LogsDirPath;
}

/*!
 * \brief Return whether there is such ortho file
 * \param orthoticId
 * \return
 */
bool FileLocationHelper::isOrthoFileExists(const QString &orthoticId)
{
    QString orthoPath = QString("%1/%2.ortho").arg(FileLocationHelper::instance()->getOrthoDir(),
                                                   orthoticId);
    return QFile::exists(orthoPath);
}

/*!
 * \brief Remove specified ortho file
 * \param orthoticId
 * \return
 */
bool FileLocationHelper::removeOrthoFile(const QString &orthoticId)
{
    QString orthoPath = QString("%1/%2.ortho").arg(FileLocationHelper::instance()->getOrthoDir(),
                                                   orthoticId);

    return QFile::remove(orthoPath);
}

/*!
 * \brief Return whether there is such scan file
 * \param scanId
 * \return
 */
bool FileLocationHelper::isScanFileExists(const QString &scanId)
{
    QString scanPath = QString("%1/%2.scan").arg(FileLocationHelper::instance()->getScanDir(),
                                                   scanId);
    return QFile::exists(scanPath);
}

/*!
 * \brief Remove specified scan file
 * \param scanId
 * \return
 */
bool FileLocationHelper::removeScanFile(const QString &scanId)
{
    QString scanPath = QString("%1/%2.scan").arg(FileLocationHelper::instance()->getScanDir(),
                                                   scanId);

    return QFile::remove(scanPath);
}

/*!
 * \brief Return ulr path for specified stl file name
 * \param stlFileName
 * \return
 */
QString FileLocationHelper::getStlUrlPath(const QString &stlFileName) const
{
    return "file:///" + m_StlDirPath + "/" + stlFileName;
}

/*!
 * \brief Return full path for specified scan file
 * \param scanId
 * \return
 */
QString FileLocationHelper::getScanFullPath(const QString &scanId) const
{
    return m_ScanDirPath + "/" + scanId + ".scan";
}

