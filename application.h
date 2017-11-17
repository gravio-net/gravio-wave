#ifndef APPLICATION_H
#define APPLICATION_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDir>
#include <QPluginLoader>
#include <QQmlExtensionPlugin>
#include <QDebug>

#include <QQuickView>
#include <QQmlContext>
#include <QSettings>
#include <QQuickStyle>

#include <QQuickItem>

#include "json.h"
#include "module.h"

/**
 * @brief Modules and app configuration
 *
 * Gravio-app home folder structure:
 *  |-data\
 *  |-logs\
 *  |-local\
 *  | |-modules\
 *  | | |--contacts\
 *  | | |...
 *  | | |--modules.config
 *  | |-gravio-app.qml
 *  |-roaming\
 *  | |-modules\
 *  | | |--contacts\
 *  | | |...
 *  | | |--modules.config
 *  | |-gravio-app.qml
 *  |-gravio-app.config
 *
 * Local & roaming modules config
 *  modules.config (JSON)
 *  {
 *          modules:
 *          [
 *              {
 *                  name: "contacts",
 *                  caption: "Contacts",
 *                  iconFile: "contacts.png",
 *                  profile: "roaming",
 *                  autoload: true
 *              },
 *              {
 *              }
 *          ]
 *  }
 *
 * Gravio-app configuration
 *  gravio-app.config (JSON)
 *  {
 *      profile: "roaming"
 *  }
 */

namespace gravio {
namespace wave {

const char APP_NAME[] = { "gravio-app" };

/**
 * @brief The Application class
 * Entry point to the gravio.app
 */
class Application : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString profile READ profile)

public:
    Application(QGuiApplication& app) : app_(app) { profile_ = "roaming"; } // may be "local"

    int Load();
    int Execute();

    QString profile() const { return profile_; }

private:
    QGuiApplication& app_;
    QQmlApplicationEngine engine_;

    QString profile_;
    QString style_;
    json::Document modulesConfig_;
    json::Document appConfig_;

    ModulesModel modules_;
};

} // wave
} // gravio

#endif // APPLICATION_H
