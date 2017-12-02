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
#include <QStandardPaths>

#include "logger.h"
#include "application.h"
#include "module.h"

//
// main
//
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication lApp(argc, argv);

    gravio::wave::gLogger.reset(new gravio::wave::Logger(gravio::wave::ApplicationPath::logsDirPath(), gravio::wave::APP_NAME));

    gravio::wave::Application lGravioApp(lApp);
    if (lGravioApp.load() > 0)
    {
        return lGravioApp.execute();
    }
    else
    {
        qWarning() << "Unexpected exit.";

    }

    return -1;
}
