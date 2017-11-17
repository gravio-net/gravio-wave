#include <QDir>
#include <QPluginLoader>
#include <QQmlExtensionPlugin>
#include <QDebug>
#include <QException>

#include "module.h"
#include "exception.h"

using namespace gravio::wave;

QString ApplicationPath::ApplicationUrlPath()
{
#ifdef Q_OS_ANDROID
    return QString("android_asset");
#endif

    return qApp->applicationDirPath();
}

QString ApplicationPath::ApplicationDirPath()
{
#ifdef Q_OS_ANDROID
    return QString("assets:");
#endif

    return qApp->applicationDirPath();
}

QString ApplicationPath::AssetUrlPath()
{
#ifdef Q_OS_ANDROID
    return QString("file:");
#endif

    return QString("file:///");
}

//
// ModuleWrapper
//

QString ModuleWrapper::Name() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"name"].GetString().c_str());
}

QString ModuleWrapper::Caption() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"caption"].GetString().c_str());
}

QString ModuleWrapper::IconFile() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"iconFile"].GetString().c_str());
}

QString ModuleWrapper::IconTitleFile() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"iconTitleFile"].GetString().c_str());
}

QString ModuleWrapper::Profile() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"profile"].GetString().c_str());
}

QString ModuleWrapper::Source() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return QString::fromStdWString(lConfig[L"source"].GetString().c_str());
}

bool ModuleWrapper::Autoload() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return lConfig[L"autoload"].GetBool();
}

int ModuleWrapper::CaptionOffset() const
{
    json::Value& lConfig = const_cast<json::Value&>(config_);
    return lConfig[L"captionOffset"].GetInt();
}

bool ModuleWrapper::IsLoaded() const
{
    return instance_.IsLoaded();
}

int ModuleWrapper::Load()
{
    return instance_.Load(Profile(), Name(), engine_);
}

QString ModuleWrapper::ModulePath() const
{
    QString lPath = ApplicationPath::ApplicationUrlPath() + "/" + Profile() + "/modules/" + Name() + "/";
    return lPath;
}

QString ModuleWrapper::IconFilePath() const
{
    QString lFileName = ApplicationPath::AssetUrlPath() + ModulePath() + IconFile();
    qDebug() << "Loading image" << lFileName;
    return lFileName;
}

QString ModuleWrapper::IconTitleFilePath() const
{
    QString lFileName = ApplicationPath::AssetUrlPath() + ModulePath() + IconTitleFile();
    qDebug() << "Loading image" << lFileName;
    return lFileName;
}

QString ModuleWrapper::SourcePath() const
{
    QString lFileName = ApplicationPath::AssetUrlPath() + ModulePath() + Source();
    qDebug() << "Returning source" << lFileName;
    return lFileName;
}

ModuleInstance& ModuleWrapper::Instance()
{
    return instance_;
}

//
// ModuleInstance
//

int ModuleInstance::Load(const QString& profile, const QString& name, QQmlApplicationEngine* engine)
{
    if (loaded_) return 1;

    QDir lModulesDir(ApplicationPath::ApplicationDirPath());    // "./"
    lModulesDir.cd(profile);                                    // "./roaming"
    lModulesDir.cd("modules");                                  // "./roaming/modules"
    lModulesDir.cd(name);                                       // "./roaming/modules/contacts"

    bool lFound = false;
    foreach (QString lFileName, lModulesDir.entryList(QDir::Files))
    {
        QPluginLoader lLoader(lModulesDir.absoluteFilePath(lFileName));
        QObject *lModule = lLoader.instance();
        if (lModule)
        {
            ((QQmlExtensionPlugin*)lModule)->initializeEngine(engine, ""); // preload module instance
            ((QQmlExtensionPlugin*)lModule)->registerTypes(""); // register module classes
            lFound = true;
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
        return modules_.at(index.row()).Name();
    case CaptionRole:
        return modules_.at(index.row()).Caption();
    case IconFileRole:
        return modules_.at(index.row()).IconFilePath();
    case ProfileRole:
        return modules_.at(index.row()).Profile();
    case AutoloadRole:
        return modules_.at(index.row()).Autoload();
    case LoadedRole:
        return modules_.at(index.row()).IsLoaded();
    case CaptionOffsetRole:
        return modules_.at(index.row()).CaptionOffset();
    case SourceRole:
        return modules_.at(index.row()).SourcePath();
    case IconTitleRole:
        return modules_.at(index.row()).IconTitleFilePath();
    case ModulePathRole:
        return modules_.at(index.row()).ModulePath();
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

    return lRoles;
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

void ModulesModel::load(int index)
{
    if (index < modules_.size())
    {
        const_cast<ModuleWrapper&>(modules_.at(index)).Load();
    }
}

const ModuleWrapper& ModulesModel::AddModule(const ModuleWrapper& module)
{
    if (index_.find(module.Name().toStdString()) == index_.end())
    {
        modules_.append(module);
        index_.insert(module.Name().toStdString());

        return modules_.at(modules_.length()-1); // return last one
    }
    else
    {
        qWarning() << "Module" << module.Name() << "already exists. Skipping...";
    }

    throw ModuleAlreadyExistsException();
}

