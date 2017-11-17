
#include <QFile>

#include "application.h"
#include "exception.h"

using namespace gravio::wave;

int Application::Load()
{
    QString lAppConfig = ApplicationPath::ApplicationDirPath() + "/" + APP_NAME + ".config";
    qInfo() << "(=================)";
    qInfo() << "Loading app-config:" << lAppConfig;

    if (QFile(lAppConfig).exists()) appConfig_.LoadFromFile(lAppConfig.toStdString());
    else
    {
        qCritical() << "File is missing" << lAppConfig << "Aborting...";
        return -2;
    }

    profile_ = QString::fromWCharArray(appConfig_.operator [](L"profile").GetString().c_str());
    style_ = QString::fromWCharArray(appConfig_.operator [](L"style").GetString().c_str());

    QString lModulesConfig = ApplicationPath::ApplicationDirPath() + "/" + profile_ + "/modules/modules.config";
    qInfo() << "Loading modules-config:" << lModulesConfig;

    if (QFile(lModulesConfig).exists()) modulesConfig_.LoadFromFile(lModulesConfig.toStdString());
    else
    {
        qCritical() << "File is missing" << lModulesConfig << "Aborting...";
        return -3;
    }

    json::Value lModules = modulesConfig_[L"modules"];
    if (lModules.IsArray())
    {
        for(int lIdx = 0; lIdx < lModules.Size(); lIdx++)
        {
            try
            {
                json::Value lModule = lModules[lIdx];
                const ModuleWrapper& lWrapper = modules_.AddModule(ModuleWrapper(lModule, &engine_));

                if (lWrapper.Autoload() && !lWrapper.IsLoaded())
                {
                    const_cast<ModuleWrapper&>(lWrapper).Load();
                }
            }
            catch(ModuleAlreadyExistsException&)
            {
                // already logged
            }
        }
    }
    else
    {
        qCritical() << "Section 'modules' was not found in" << lModulesConfig;
        return -1;
    }

    return appConfig_.HasErrors() ? -1 : 1;
}

int Application::Execute()
{
    QQuickStyle::setStyle(style_); // default style

    engine_.rootContext()->setContextProperty("gravioApp", this);
    engine_.rootContext()->setContextProperty("modulesModel", &modules_);
    engine_.rootContext()->setContextProperty("availableStyles", QQuickStyle::availableStyles());

    engine_.load(ApplicationPath::ApplicationDirPath() + "/" + profile_ + "/" + APP_NAME + ".qml");
    if (engine_.rootObjects().isEmpty())
    {
        qCritical() << "Root object is empty. Exiting...";
        return -1;
    }

    return app_.exec();
}
