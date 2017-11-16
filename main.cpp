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

//
// main
//
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication lApp(argc, argv);

    gravio::wave::gLogger.reset(new gravio::wave::Logger(qApp->applicationDirPath() + "/logs", gravio::wave::APP_NAME));

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

/*

    QDir pluginsDir(qApp->applicationDirPath());
    //pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
            ((QQmlExtensionPlugin*)plugin)->registerTypes("");
            qDebug() << "module loaded";
        }
    }


    //QString lPath = "C:\\Projects\\gravio-wave\\build\\debug";

    QQmlApplicationEngine engine;
    engine.addImportPath(qApp->applicationDirPath());
    engine.load(qApp->applicationDirPath() + QLatin1String("/roaming/gravio-app.qml"));
    if (engine.rootObjects().isEmpty())
    {
        qDebug() << "not found";
        return -1;
    }

    return app.exec();
    */
