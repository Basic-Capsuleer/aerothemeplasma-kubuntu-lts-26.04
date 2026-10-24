#ifndef KCM_AEROSHELLSDDM_H
#define KCM_AEROSHELLSDDM_H

#include <QObject>
#include <QQuickItem>
#include <QUrl>

#include <KSharedConfig>
#include <KQuickManagedConfigModule>

class ThemesModel;

class KCMAeroShellSDDM : public KQuickManagedConfigModule
{
    Q_OBJECT

    Q_PROPERTY(ThemesModel *themesModel READ themesModel)

public:
    explicit KCMAeroShellSDDM(QObject *parent, const KPluginMetaData &data);
    ~KCMAeroShellSDDM() override;

    ThemesModel *themesModel();

public Q_SLOTS:
    void updateVars();

    void setupQmlConnections();
    void checkNeedsSave();

protected Q_SLOTS:
    void load() override;
    void save() override;

private:
    QString m_defaultConfig;
    QString m_userConfig;
    QString m_background;
    QString m_defaultBackground;
    QString m_currentBackground;

    KSharedConfig::Ptr m_config;

    bool m_defaultsLoaded;
    ThemesModel *m_themesModel{nullptr};
};

#endif
