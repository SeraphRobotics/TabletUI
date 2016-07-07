#ifndef FILELOCATIONHELPER_H
#define FILELOCATIONHELPER_H

#include <QString>

class FileLocationHelper
{
public:
    static FileLocationHelper* instance();

    QString getScanDir() const;
    QString getOrthoDir() const;
    QString getStlDir() const;
    QString getLogsDir() const;

    QString getStlUrlPath(const QString &stlFileName) const;
    QString getScanFullPath(const QString &scanId) const;

    static bool isOrthoFileExists(const QString &orthoticId);
    static bool removeOrthoFile(const QString &orthoticId);

    static bool isScanFileExists(const QString &scanId);
    static bool removeScanFile(const QString &scanId);

private:
    FileLocationHelper();
    // disable copy
    FileLocationHelper(const FileLocationHelper &) = delete;
    FileLocationHelper &operator=(const FileLocationHelper &) = delete;

    static FileLocationHelper *m_Instance;

    QString m_ScanDirPath;
    QString m_OrthoDirPath;
    QString m_StlDirPath;
    QString m_LogsDirPath;
};

#endif // FILELOCATIONHELPER_H
