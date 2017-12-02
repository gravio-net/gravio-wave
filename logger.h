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

    void logInfo(const QString& msg);
    void logInfo(const QString& category, const QString& msg);

    void logWarn(const QString& msg);
    void logWarn(const QString& category, const QString& msg);

    void logDebug(const QString& msg);
    void logDebug(const QString& category, const QString& msg);

    void logCritical(const QString& msg);
    void logCritical(const QString& category, const QString& msg);

    void logFatal(const QString& msg);
    void logFatal(const QString& category, const QString& msg);

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
