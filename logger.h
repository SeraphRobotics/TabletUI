#ifndef LOGGER_H
#define LOGGER_H

#include <QDate>

class QFile;
class QMutex;

class Logger
{
public:
    static Logger *instance();

    ~Logger();

    void write(QtMsgType type, const QString &msg);
    void setLogsDirPath(const QString &logsDirPath);
    void setLogFileNameTemplate(const QString &logFileNameTemplate);

private:
    Logger();
    // disable copy
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    static Logger *m_instance;

    QFile *m_file = nullptr;
    QMutex *m_mutex = nullptr;
    QDate m_lastCallDate;
    QString m_logsDirPath;
    QString m_logFileNameTemplate;
};

#endif // LOGGER_H
