#include "kcm.h"

#include "themesmodel.h"

#include <QFileInfo>
#include <QQuickWindow>
#include <QWindow>

#include <KPluginFactory>
#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KCoreConfigSkeleton>

using namespace KAuth;

K_PLUGIN_CLASS_WITH_JSON(KCMAeroShellSDDM, "kcm_aeroshellsddm.json")

KCMAeroShellSDDM::KCMAeroShellSDDM(QObject *parent, const KPluginMetaData &data)
    : KQuickManagedConfigModule(parent, data)
    , m_defaultsLoaded{false}
    , m_themesModel{new ThemesModel(this)}
{
    setAuthActionName("aeroshell.kcmaeroshellsddm.write");

    setSupportsInstantApply(false);

    connect(this, &KQuickConfigModule::mainUiReady, this, &KCMAeroShellSDDM::updateVars);
    connect(this, &KQuickConfigModule::mainUiReady, this, &KCMAeroShellSDDM::setupQmlConnections);
}

KCMAeroShellSDDM::~KCMAeroShellSDDM() = default;


ThemesModel *KCMAeroShellSDDM::themesModel()
{ return m_themesModel; }


void KCMAeroShellSDDM::updateVars()
{
    QString path = mainUi()->property("themeToConfigure").toString();

    m_defaultConfig      = path + "/theme.conf";
    m_userConfig         = path + "/theme.conf.user";
    m_background         = path + "/background";
    m_defaultBackground  = path + "/default-background";
    m_currentBackground  = m_defaultBackground;

    m_config = KSharedConfig::openConfig(m_defaultConfig);
    if(QFileInfo::exists(m_userConfig)) {
        m_config->copyFrom(KConfig(m_userConfig));
    }

    load();
}


void KCMAeroShellSDDM::setupQmlConnections()
{
    connect(mainUi(), SIGNAL(changed()), this, SLOT(checkRepresentsDefaults()));
    connect(mainUi(), SIGNAL(changed()), this, SLOT(checkNeedsSave()));
    connect(mainUi(), SIGNAL(themeToConfigureChanged()), this, SLOT(updateVars()));
}

void KCMAeroShellSDDM::checkNeedsSave()
{
    KConfigGroup general = m_config->group("General");

    // i love this specific part of the code fr
    setNeedsSave(mainUi()->property("enableStartup").toBool() != general.readEntry("enableStartup", QVariant(true)).toBool()
                 || mainUi()->property("playSound").toBool() != general.readEntry("playSound", QVariant(true)).toBool()
                 || mainUi()->property("forceUserSelect").toBool() != general.readEntry("forceUserSelect", QVariant(false)).toBool()
                 || mainUi()->property("rdpBackground").toBool() != general.readEntry("rdpBackground", QVariant(false)).toBool()
                 || mainUi()->property("backgroundSrc").toUrl() != QUrl::fromLocalFile(m_currentBackground));
}


void KCMAeroShellSDDM::load()
{
    KConfigGroup general = m_config->group("General");

    mainUi()->setProperty("enableStartup", general.readEntry("enableStartup", false));
    mainUi()->setProperty("playSound", general.readEntry("playSound", true));
    mainUi()->setProperty("forceUserSelect", general.readEntry("forceUserSelect", false));
    mainUi()->setProperty("rdpBackground", general.readEntry("rdpBackground", false));

    if(!QFileInfo::exists(m_background)) {
        m_currentBackground = m_defaultBackground;
    } else {
        m_currentBackground = m_background;
    }

    mainUi()->setProperty("backgroundSrc", QVariant(QUrl::fromLocalFile(m_currentBackground)));
}

void KCMAeroShellSDDM::save()
{
    QVariantMap args;

    args["path"]            = mainUi()->property("themeToConfigure").toString();
    args["enableStartup"]   = mainUi()->property("enableStartup").toBool();
    args["playSound"]       = mainUi()->property("playSound").toBool();
    args["forceUserSelect"] = mainUi()->property("forceUserSelect").toBool();
    args["rdpBackground"]   = mainUi()->property("rdpBackground").toBool();
    args["background"]      = mainUi()->property("backgroundSrc").toUrl();

    Action writeAction(QStringLiteral("aeroshell.kcmaeroshellsddm.write"));
    writeAction.setHelperId(QStringLiteral("aeroshell.kcmaeroshellsddm"));
    writeAction.setArguments(args);

    ExecuteJob *job = writeAction.execute();
    if(!job->exec()) {
        setNeedsSave(true);
    } else {
        setNeedsSave(false);
        updateVars();
    }
}

#include "kcm.moc"
#include "moc_kcm.cpp"
