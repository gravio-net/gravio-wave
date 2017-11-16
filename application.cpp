
#include "application.h"

using namespace gravio::wave;

int Application::Load()
{
    QString lAppConfig = qApp->applicationDirPath() + "/" + APP_NAME + ".config";
    qInfo() << "(=================)";
    qInfo() << "Loading app-config:" << lAppConfig;
    appConfig_.LoadFromFile(lAppConfig.toStdString());

    profile_ = QString::fromWCharArray(appConfig_.operator [](L"profile").GetString().c_str());

    QString lModulesConfig = qApp->applicationDirPath() + "/" + profile_ + "/modules/modules.config";
    qInfo() << "Loading modules-config:" << lModulesConfig;
    modulesConfig_.LoadFromFile(lModulesConfig.toStdString());

    json::Value lModules = modulesConfig_[L"modules"];
    if (lModules.IsArray())
    {
        for(int lIdx = 0; lIdx < lModules.Size(); lIdx++)
        {
            json::Value lModule = lModules[lIdx];
            ModuleWrapper lWrapper(lModule);
            modules_.AddModule(lWrapper);

            if (lWrapper.Autoload() && !lWrapper.IsLoaded())
            {
                lWrapper.Load(engine_);
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
    /*
    QSettings lSettings;
    QString lStyle = QQuickStyle::name();
    if (!lStyle.isEmpty())
        lSettings.setValue("style", lStyle);
    else
        QQuickStyle::setStyle(lSettings.value("style").toString());
    */

    QQuickStyle::setStyle("Material"); // default style

    engine_.rootContext()->setContextProperty("gravioApp", this);
    engine_.rootContext()->setContextProperty("modulesModel", &modules_);
    engine_.rootContext()->setContextProperty("availableStyles", QQuickStyle::availableStyles());

    engine_.load(qApp->applicationDirPath() + "/" + profile_ + "/" + APP_NAME + ".qml");
    if (engine_.rootObjects().isEmpty())
    {
        qCritical() << "Root object is empty. Exiting...";
        return -1;
    }

    return app_.exec();
}
