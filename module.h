#ifndef MODULE_H
#define MODULE_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QAbstractListModel>
#include <QList>
#include <QByteArray>
#include <QModelIndex>
#include <QHash>
#include <QVariant>
#include <QStandardPaths>

#include <set>

#include "json.h"

namespace gravio {
namespace wave {

/**
 * @brief The ApplicationPath class
 * AppDir for the various platforms
 */
class ApplicationPath
{
public:
    static QString applicationDirPath();
    static QString applicationUrlPath()
    {
    #ifdef Q_OS_ANDROID
        return QString("assets:");
    #endif

        return qApp->applicationDirPath();
    }
    static  QString assetUrlPath()
    {
    #ifdef Q_OS_ANDROID
        return QString("file:");
    #endif

        return QString("file:///");
    }
    static QString dataDirPath()
    {
    #ifdef Q_OS_ANDROID
        return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    #endif

        return qApp->applicationDirPath() + "/data";
    }
    static QString logsDirPath();
};

/**
 * @brief The ModuleInstance class
 * Wrapper for the really loaded modle
 */
class ModuleInstance
{
public:
    ModuleInstance() : name_("default"), loaded_(false) {}

    bool isLoaded() const { return loaded_; }
    int load(const QString& profile, const QString& name, QQmlApplicationEngine* engine);

private:
    QString prepareBin(const QString&);
    bool isExecutable(QString&);

private:
    QString name_;
    bool loaded_;
};

/**
 * @brief The ModuleWrapper class
 * Wrapper for the moule
 */
class ModuleWrapper
{
public:
    ModuleWrapper(json::Value& config, QQmlApplicationEngine* engine): config_(config), engine_(engine) {}

    QString name() const;
    QString caption() const;
    QString iconFile() const;
    QString iconTitleFile() const;
    QString profile() const;
    QString source() const;
    int captionOffset() const;
    int iconLeftPadding() const;
    int iconTitleTopPadding() const;
    bool autoload() const;
    bool isLoaded() const;
    QString modulePath() const;
    QString assetPrefix() const;
    QString iconFilePath() const;
    QString iconTitleFilePath() const;
    QString sourcePath() const;
    QString moduleDataPath() const;

    int load();

    ModuleInstance& instance();

private:
    json::Value config_;
    ModuleInstance instance_;
    QQmlApplicationEngine* engine_;
};


/**
 * @brief The ModulesModel class
 * List source for the pane
 */
class ModulesModel: public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModulesRoles
    {
        NameRole = Qt::UserRole + 1,
        CaptionRole,
        IconFileRole,
        ProfileRole,
        AutoloadRole,
        LoadedRole,
        CaptionOffsetRole,
        SourceRole,
        IconTitleRole,
        ModulePathRole,
        IconLeftPaddingRole,
        IconTitleTopPaddingRole,
        AssetPrefixRole,
        ModuleDataPathRole
    };

    ModulesModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE QVariantMap get(int index);
    Q_INVOKABLE void load(int index);

    const ModuleWrapper& addModule(const ModuleWrapper&);

private:
    QList<ModuleWrapper> modules_;
    std::set<std::string> index_;
};

} // wave
} // gravio

#endif // MODULE_H
