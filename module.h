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

#include <set>

#include "json.h"

namespace gravio {
namespace wave {

/**
 * @brief The ModuleInstance class
 * Wrapper for the really loaded modle
 */
class ModuleInstance
{
public:
    ModuleInstance() : name_("default"), loaded_(false) {}

    bool IsLoaded() const { return loaded_; }
    int Load(const QString& profile, const QString& name, QQmlApplicationEngine* engine);

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

    QString Name() const;
    QString Caption() const;
    QString IconFile() const;
    QString IconTitleFile() const;
    QString Profile() const;
    QString Source() const;
    int CaptionOffset() const;
    bool Autoload() const;
    bool IsLoaded() const;
    QString ModulePath() const;
    QString IconFilePath() const;
    QString IconTitleFilePath() const;
    QString SourcePath() const;

    int Load();

    ModuleInstance& Instance();

private:
    json::Value config_;
    ModuleInstance instance_;
    QQmlApplicationEngine* engine_;
};


/**
 * @brief The ModulesModel class
 * List source for the pane
 */
class ModulesModel : public QAbstractListModel
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
        ModulePathRole
    };

    ModulesModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE QVariantMap get(int index);
    Q_INVOKABLE void load(int index);

    const ModuleWrapper& AddModule(const ModuleWrapper&);

private:
    QList<ModuleWrapper> modules_;
    std::set<std::string> index_;
};

} // wave
} // gravio

#endif // MODULE_H
