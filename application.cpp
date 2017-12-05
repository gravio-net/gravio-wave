
#include <QFile>

#include "application.h"
#include "exception.h"

using namespace gravio::wave;

int Application::load()
{
    try
    {
        QString lAppConfig = ApplicationPath::applicationDirPath() + "/" + APP_NAME + ".config";
        qInfo() << "(=================)";
        qInfo() << "Loading app-config:" << lAppConfig;

        if (QFile(lAppConfig).exists()) appConfig_.loadFromFile(lAppConfig.toStdString());
        else
        {
            qCritical() << "File is missing" << lAppConfig << "Aborting...";
            return -2;
        }

        profile_ = QString::fromWCharArray(appConfig_.operator [](L"profile").getString().c_str());
        style_ = QString::fromWCharArray(appConfig_.operator [](L"style").getString().c_str());

        QString lModulesConfig = ApplicationPath::applicationDirPath() + "/" + profile_ + "/modules/modules.config";
        qInfo() << "Loading modules-config:" << lModulesConfig;

        if (QFile(lModulesConfig).exists()) modulesConfig_.loadFromFile(lModulesConfig.toStdString());
        else
        {
            qCritical() << "File is missing" << lModulesConfig << "Aborting...";
            return -3;
        }

        json::Value lModules = modulesConfig_[L"modules"];
        if (lModules.isArray())
        {
            for(size_t lIdx = 0; lIdx < lModules.size(); lIdx++)
            {
                try
                {
                    json::Value lModule = lModules[lIdx];
                    const ModuleWrapper& lWrapper = modules_.addModule(ModuleWrapper(lModule, &engine_));

                    if (lWrapper.autoload() && !lWrapper.isLoaded())
                    {
                        const_cast<ModuleWrapper&>(lWrapper).load();
                    }
                }
                catch(ModuleAlreadyExistsException const&)
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
    }
    catch(wave::Exception const& ex)
    {
        qCritical() << ex.message().c_str();
        return -1;
    }

    return appConfig_.hasErrors() ? -1 : 1;
}

int Application::execute()
{
    QQuickStyle::setStyle(style_); // default style

    qmlRegisterType<gravio::wave::Account>("net.gravio.wave.account", 1, 0, "Account");
    qmlRegisterType<gravio::wave::AccountAddress>("net.gravio.wave.account", 1, 0, "AccountAddress");
    qmlRegisterType<gravio::wave::AccountAddresses>("net.gravio.wave.account", 1, 0, "AccountAddresses");

    engine_.rootContext()->setContextProperty("gravioApp", this);
    engine_.rootContext()->setContextProperty("gravioAccount", &account_);
    engine_.rootContext()->setContextProperty("modulesModel", &modules_);
    engine_.rootContext()->setContextProperty("appHelper", &helper_);
    engine_.rootContext()->setContextProperty("availableStyles", QQuickStyle::availableStyles());

    engine_.load(ApplicationPath::applicationDirPath() + "/" + profile_ + "/" + APP_NAME + ".qml");
    if (engine_.rootObjects().isEmpty())
    {
        qCritical() << "Root object is empty. Exiting...";
        return -1;
    }

    return app_.exec();
}
