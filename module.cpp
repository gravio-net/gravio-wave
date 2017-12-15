#include <QDir>
#include <QPluginLoader>
#include <QQmlExtensionPlugin>
#include <QDebug>
#include <QException>
#include <QStandardPaths>
#include <QStringList>
#include <QJsonDocument>

#include "module.h"
#include "exception.h"
#include "imoduleplugin.h"

using namespace gravio::wave;

#ifdef Q_OS_ANDROID
std::wstring gProfile(L"android");
#else
std::wstring gProfile(L"desktop");
#endif

QString ApplicationPath::applicationDirPath()
{
#ifdef Q_OS_ANDROID
    return QString("assets:");
#endif

    return qApp->applicationDirPath();
}

QString ApplicationPath::logsDirPath()
{
#ifdef Q_OS_ANDROID
    return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
#endif

    return qApp->applicationDirPath() + "/logs";
}

//
// ModuleWrapper
//

QString ModuleWrapper::name() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"name"].getString().c_str());
}

QString ModuleWrapper::caption() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"caption"].getString().c_str());
}

QString ModuleWrapper::iconFile() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"iconFile"].getString().c_str());
}

QString ModuleWrapper::iconTitleFile() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"iconTitleFile"].getString().c_str());
}

QString ModuleWrapper::profile() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"profile"].getString().c_str());
}

QString ModuleWrapper::source() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"source"].getString().c_str());
}

bool ModuleWrapper::autoload() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return lConfig[L"autoload"].getBool();
}

int ModuleWrapper::captionOffset() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);

    json::Value lProfiles, lSettings;
    if(lConfig.find(L"profiles", lProfiles) && lProfiles.find(gProfile, lSettings))
    {
        return lSettings[L"captionOffset"].getInt();
    }

    return lConfig[L"captionOffset"].getInt();
}

int ModuleWrapper::iconLeftPadding() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);

    json::Value lProfiles, lSettings;
    if(lConfig.find(L"profiles", lProfiles) && lProfiles.find(gProfile, lSettings))
    {
        return lSettings[L"iconLeftPadding"].getInt();
    }

    return lConfig[L"iconLeftPadding"].getInt();
}

int ModuleWrapper::iconTitleTopPadding() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);

    json::Value lProfiles, lSettings;
    if(lConfig.find(L"profiles", lProfiles) && lProfiles.find(gProfile, lSettings))
    {
        return lSettings[L"iconTitleTopPadding"].getInt();
    }

    return lConfig[L"iconTitleTopPadding"].getInt();
}

bool ModuleWrapper::isLoaded() const
{
    return instance_.isLoaded();
}

int ModuleWrapper::load()
{
    return instance_.load(profile(), name(), application_);
}

QString ModuleWrapper::modulePath() const
{
    QString lPath = ApplicationPath::applicationUrlPath() + "/" + profile() + "/modules/" + name() + "/";
    return lPath;
}

QString ModuleWrapper::assetPrefix() const
{
    return ApplicationPath::assetUrlPath();
}

QString ModuleWrapper::iconFilePath() const
{
    QString lFileName = ApplicationPath::assetUrlPath() + modulePath() + iconFile();
    qDebug() << "Loading image" << lFileName;
    return lFileName;
}

QString ModuleWrapper::iconTitleFilePath() const
{
    QString lFileName = ApplicationPath::assetUrlPath() + modulePath() + iconTitleFile();
    qDebug() << "Loading image" << lFileName;
    return lFileName;
}

QString ModuleWrapper::sourcePath() const
{
    QString lFileName = ApplicationPath::assetUrlPath() + modulePath() + source();
    qDebug() << "Returning source" << lFileName;
    return lFileName;
}

QString ModuleWrapper::moduleDataPath() const
{
    QString lDataPath = ApplicationPath::dataDirPath() + "/" + name();
    qDebug() << "Returning data path" << lDataPath;
    return lDataPath;
}


ModuleInstance& ModuleWrapper::instance()
{
    return instance_;
}

//
// ModuleInstance
//

QString ModuleInstance::prepareBin(const QString& name)
{
    QString lGlobalDataPath = ApplicationPath::dataDirPath();
    QString lModuleDataPath = lGlobalDataPath + "/" + name;

    QDir lCurrentDataDir(lModuleDataPath);
    if (!lCurrentDataDir.exists())
    {
        lCurrentDataDir.setPath(lGlobalDataPath);
        lCurrentDataDir.mkdir(name);
    }

    return lModuleDataPath;
}

bool ModuleInstance::isExecutable(QString& fileName)
{
    QStringList lParts = fileName.split(".");
    if (lParts.length() > 0 && (lParts[lParts.length()-1] == "so" || lParts[lParts.length()-1] == "dll")) return true;
    return false;
}

int ModuleInstance::load(const QString& profile, const QString& name, IApplication* application)
{
    if (loaded_) return 1;

    QDir lModulesDir(ApplicationPath::applicationDirPath());    // "./"
    lModulesDir.cd(profile);                                    // "./roaming"
    lModulesDir.cd("modules");                                  // "./roaming/modules"
    lModulesDir.cd(name);                                       // "./roaming/modules/contacts"

    bool lFound = false;
    foreach (QString lFileName, lModulesDir.entryList(QDir::Files))
    {
        if (isExecutable(lFileName))
        {
            QPluginLoader lLoader;
#ifdef Q_OS_ANDROID
            QString lRunLocation = prepareBin(name);
            QString lDestFile = lRunLocation + "/" + lFileName;

            if (QFile::exists(lDestFile)) QFile::remove(lDestFile);

            if (!QFile::exists(lDestFile))
            {
                QFile::copy(lModulesDir.absoluteFilePath(lFileName), lDestFile);
                QFile::setPermissions(lDestFile, QFile::ExeOther|QFile::ExeGroup|QFile::ExeUser|QFile::ExeOwner|QFile::WriteOwner|QFile::ReadOwner);
            }

            qDebug() << "Trying to load" << lDestFile;
            lLoader.setFileName(lDestFile);
#else
            qDebug() << "Trying to load" << lModulesDir.absoluteFilePath(lFileName);
            lLoader.setFileName(lModulesDir.absoluteFilePath(lFileName));
#endif

            QJsonDocument doc(lLoader.metaData());
            QString strJson(doc.toJson(QJsonDocument::Compact));
            qDebug() << strJson;

            if (!lLoader.load())
            {
                qDebug() << "NOT loaded:" << lLoader.errorString();
            }
            else qDebug() << "LOADED";

            QObject *lModule = lLoader.instance();
            if (lModule)
            {
                ((QQmlExtensionPlugin*)lModule)->initializeEngine(application->getEngine(), ""); // preload module instance
                ((QQmlExtensionPlugin*)lModule)->registerTypes(""); // register module classes

                IModulePlugin* lMetaModule = dynamic_cast<gravio::wave::IModulePlugin*>(lModule);
                lMetaModule->initializeModule(application->getAccount());

                lFound = true;
            }
        }
    }

    if (lFound) loaded_ = true;

    return lFound ? 1 : 0;
}

//
// ModulesModel
//

ModulesModel::ModulesModel(QObject *parent): QAbstractListModel(parent)
{
}

int ModulesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return modules_.size();
}

QVariant ModulesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
    case NameRole:
        return modules_.at(index.row()).name();
    case CaptionRole:
        return modules_.at(index.row()).caption();
    case IconFileRole:
        return modules_.at(index.row()).iconFilePath();
    case ProfileRole:
        return modules_.at(index.row()).profile();
    case AutoloadRole:
        return modules_.at(index.row()).autoload();
    case LoadedRole:
        return modules_.at(index.row()).isLoaded();
    case CaptionOffsetRole:
        return modules_.at(index.row()).captionOffset();
    case SourceRole:
        return modules_.at(index.row()).sourcePath();
    case IconTitleRole:
        return modules_.at(index.row()).iconTitleFilePath();
    case ModulePathRole:
        return modules_.at(index.row()).modulePath();
    case IconLeftPaddingRole:
        return modules_.at(index.row()).iconLeftPadding();
    case IconTitleTopPaddingRole:
        return modules_.at(index.row()).iconTitleTopPadding();
    case AssetPrefixRole:
        return modules_.at(index.row()).assetPrefix();
    case ModuleDataPathRole:
        return modules_.at(index.row()).moduleDataPath();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ModulesModel::roleNames() const
{
    QHash<int, QByteArray> lRoles = QAbstractListModel::roleNames();
    lRoles[NameRole] = "name";
    lRoles[CaptionRole] = "caption";
    lRoles[IconFileRole] = "iconFile";
    lRoles[ProfileRole] = "profile";
    lRoles[AutoloadRole] = "autoload";
    lRoles[LoadedRole] = "loaded";
    lRoles[CaptionOffsetRole] = "captionOffset";
    lRoles[SourceRole] = "source";
    lRoles[IconTitleRole] = "iconTitleFile";
    lRoles[ModulePathRole] = "modulePath";
    lRoles[IconLeftPaddingRole] = "iconLeftPadding";
    lRoles[IconTitleTopPaddingRole] = "iconTitleTopPadding";
    lRoles[AssetPrefixRole] = "assetPrefix";
    lRoles[ModuleDataPathRole] = "moduleDataPath";

    return lRoles;
}

ModuleWrapper* ModulesModel::find(QString& name)
{
    int lIdx = findIndex(name);
    if (lIdx < 0) return 0;

    return (ModuleWrapper*)&modules_.at(lIdx);
}

int ModulesModel::findIndex(QString name)
{
    QMap<QString, int>::iterator lIterator = index_.find(name);
    if (lIterator == index_.end()) return -1;

    return lIterator.value();
}

QVariantMap ModulesModel::get(int row)
{
    if (row < modules_.size())
    {
        QHash<int,QByteArray> lNames = roleNames();
        QHashIterator<int, QByteArray> lIterator(lNames);
        QVariantMap lRes;
        while (lIterator.hasNext())
        {
            lIterator.next();
            QModelIndex lIdx = index(row, 0);
            QVariant lData = lIdx.data(lIterator.key());
            lRes[lIterator.value()] = lData;
        }

        return lRes;
    }

    return QVariantMap();
}

QVariantMap ModulesModel::get(QString name)
{
    int lRow = findIndex(name);

    if (lRow < modules_.size())
    {
        QHash<int,QByteArray> lNames = roleNames();
        QHashIterator<int, QByteArray> lIterator(lNames);
        QVariantMap lRes;
        while (lIterator.hasNext())
        {
            lIterator.next();
            QModelIndex lIdx = index(lRow, 0);
            QVariant lData = lIdx.data(lIterator.key());
            lRes[lIterator.value()] = lData;
        }

        return lRes;
    }

    return QVariantMap();
}

void ModulesModel::load(int index)
{
    if (index < modules_.size())
    {
        const_cast<ModuleWrapper&>(modules_.at(index)).load();
    }
}

void ModulesModel::load(QString name)
{
    ModuleWrapper* lWrapper = find(name);

    if (lWrapper)
    {
        lWrapper->load();
    }
}

const ModuleWrapper& ModulesModel::addModule(const ModuleWrapper& module)
{
    if (index_.find(module.name()) == index_.end())
    {
        modules_.append(module);
        index_.insert(module.name(), modules_.length()-1);

        return modules_.at(modules_.length()-1); // return last one
    }
    else
    {
        qWarning() << "Module" << module.name() << "already exists. Skipping...";
    }

    throw ModuleAlreadyExistsException();
}

