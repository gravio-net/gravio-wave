#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDir>
#include <QPluginLoader>
#include <QQmlExtensionPlugin>
#include <QDebug>

#include <QFile>
#include <QScopedPointer>
#include <QTextStream>
#include <QDateTime>

#include "logger.h"
#include "application.h"
#include "module.h"
#include <QStandardPaths>
//
// main
//
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication lApp(argc, argv);

    gravio::wave::gLogger.reset(new gravio::wave::Logger(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), gravio::wave::APP_NAME));

    gravio::wave::Application lGravioApp(lApp);
    if (lGravioApp.Load() > 0)
    {
        return lGravioApp.Execute();
    }
    else
    {
        qWarning() << "Unexpected exit.";

    }

    return -1;
}
