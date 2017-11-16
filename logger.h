#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QScopedPointer>
#include <QTextStream>
#include <QDateTime>

namespace gravio {
namespace wave {

extern void LoggerMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

/**
 * @brief The Logger class
 * Logger for the Qt Application
 */
class Logger
{
public:
    Logger(QString path, QString pattern);

    void LogInfo(const QString& msg);
    void LogInfo(const QString& category, const QString& msg);

    void LogWarn(const QString& msg);
    void LogWarn(const QString& category, const QString& msg);

    void LogDebug(const QString& msg);
    void LogDebug(const QString& category, const QString& msg);

    void LogCritical(const QString& msg);
    void LogCritical(const QString& category, const QString& msg);

    void LogFatal(const QString& msg);
    void LogFatal(const QString& category, const QString& msg);

    void Log(QtMsgType type, const QString& category, const QString& msg);

private:
    QString path_;
    QString pattern_;
    QScopedPointer<QFile> file_;
};

extern QScopedPointer<Logger> gLogger;

} // wave
} // gravio

#endif // LOGGER_H
