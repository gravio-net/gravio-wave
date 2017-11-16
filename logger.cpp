#include "logger.h"

using namespace gravio::wave;

QScopedPointer<Logger> gravio::wave::gLogger;

void gravio::wave::LoggerMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (!gLogger.isNull()) gLogger->Log(type, context.category, msg);
}

Logger::Logger(QString path, QString pattern): path_(path), pattern_(pattern)
{
    qInstallMessageHandler(gravio::wave::LoggerMessageHandler);
}

void Logger::LogInfo(const QString& msg)
{
    static const QString lCommon("common");
    LogInfo(lCommon, msg);
}
void Logger::LogInfo(const QString& category, const QString& msg)
{
    Log(QtMsgType::QtInfoMsg, category, msg);
}

void Logger::LogWarn(const QString& msg)
{
    static const QString lCommon("common");
    LogInfo(lCommon, msg);
}
void Logger::LogWarn(const QString& category, const QString& msg)
{
    Log(QtMsgType::QtWarningMsg, category, msg);
}

void Logger::LogDebug(const QString& msg)
{
    static const QString lCommon("common");
    LogInfo(lCommon, msg);
}
void Logger::LogDebug(const QString& category, const QString& msg)
{
    Log(QtMsgType::QtDebugMsg, category, msg);
}

void Logger::LogCritical(const QString& msg)
{
    static const QString lCommon("common");
    LogInfo(lCommon, msg);
}
void Logger::LogCritical(const QString& category, const QString& msg)
{
    Log(QtMsgType::QtCriticalMsg, category, msg);
}

void Logger::LogFatal(const QString& msg)
{
    static const QString lCommon("common");
    LogInfo(lCommon, msg);
}
void Logger::LogFatal(const QString& category, const QString& msg)
{
    Log(QtMsgType::QtFatalMsg, category, msg);
}

void Logger::Log(QtMsgType type, const QString& category, const QString& msg)
{
    if (file_.isNull())
    {
        file_.reset(new QFile(path_ + "/" + pattern_ + ".log"));
        file_.data()->open(QFile::Append | QFile::Text);
    }
    //else if (date_changed) rotate();

    QTextStream lOut(file_.data());
    lOut << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");

    switch (type)
    {
        case QtInfoMsg:     lOut << "[inf]"; break;
        case QtDebugMsg:    lOut << "[dbg]"; break;
        case QtWarningMsg:  lOut << "[wrn]"; break;
        case QtCriticalMsg: lOut << "[crt]"; break;
        case QtFatalMsg:    lOut << "[ftl]"; break;
    }

    lOut << "[" << category << "]: " << msg << endl;
    lOut.flush();
}

