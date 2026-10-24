#include "helper.h"

#include <QFile>
#include <QFileInfo>
#include <QUrl>

#include <KConfig>
#include <KConfigGroup>

ActionReply KCMHelper::write(const QVariantMap &args)
{
    ActionReply reply;

    m_path = args[QStringLiteral("path")].toString();
    updateVars();

    bool enableStartup = args[QStringLiteral("enableStartup")].toBool();
    bool playSound = args[QStringLiteral("playSound")].toBool();
    bool forceUserSelect = args[QStringLiteral("forceUserSelect")].toBool();
    bool rdpBackground = args[QStringLiteral("rdpBackground")].toBool();
    QString background = args[QStringLiteral("background")].toUrl().toLocalFile();

    if(background != m_background && !rdpBackground) {
        // get rid of the already existing one first
        if(QFileInfo::exists(m_background)) {
            QFile fi(m_background);

            if(!fi.remove()) {
                reply = ActionReply::HelperErrorReply();
                reply.setErrorDescription(fi.errorString());
                qDebug() << "fail to set background:" << fi.errorString();
                return reply;
            }
        }

        QFile file(background);

        // then copy the new one
        // doing this because the copy function doesnt overwrite and will return false instead
        if(!file.copy(m_background)) {
            reply = ActionReply::HelperErrorReply();
            reply.setErrorDescription(file.errorString());
            qDebug() << "fail to set background:" << file.errorString();
            return reply;
        }
    }

    if(rdpBackground) {
        QFile file(m_background);

        if(!file.remove(m_background)) {
            reply = ActionReply::HelperErrorReply();
            reply.setErrorDescription(file.errorString());
            qDebug() << "fail to set property rdpBackground:" << file.errorString();
            return reply;
        }

        qDebug("successfully set rdp background");
    }

    KConfig cfg(m_userConfig);

    KConfig prevCfg(m_userConfig);
    KConfigGroup pgroup_cfg = prevCfg.group(QStringLiteral("General"));

    KConfigGroup group_cfg = cfg.group(QStringLiteral("General"));
    if(enableStartup != pgroup_cfg.readEntry(QStringLiteral("enableStartup"), QVariant(false)).toBool()) {
        group_cfg.writeEntry(QStringLiteral("enableStartup"), enableStartup);
    }
    if(playSound != pgroup_cfg.readEntry(QStringLiteral("playSound"), QVariant(true)).toBool()) {
        group_cfg.writeEntry(QStringLiteral("playSound"), playSound);
    }
    if(forceUserSelect != pgroup_cfg.readEntry(QStringLiteral("forceUserSelect"), QVariant(false)).toBool()) {
        group_cfg.writeEntry(QStringLiteral("forceUserSelect"), forceUserSelect);
    }
    if(rdpBackground != pgroup_cfg.readEntry(QStringLiteral("rdpBackground"), QVariant(false)).toBool()) {
        group_cfg.writeEntry(QStringLiteral("rdpBackground"), rdpBackground);
    }

    group_cfg.copyTo(&cfg);

    if(cfg.sync()) {
        qDebug("config synced succesfully");
        QFile userCfg(m_userConfig);
        userCfg.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                               QFileDevice::ReadUser |
                               QFileDevice::ReadGroup |
                               QFileDevice::ReadOther);
        return reply;
    }

    reply = ActionReply::HelperErrorReply();
    reply.setErrorDescription(QStringLiteral("Could not sync configuration"));
    qDebug("fail to sync config");
    return reply;
}

void KCMHelper::updateVars()
{
    m_defaultConfig      = m_path + "/theme.conf";
    m_userConfig         = m_path + "/theme.conf.user";
    m_background         = m_path + "/background";
}

KAUTH_HELPER_MAIN("aeroshell.kcmaeroshellsddm", KCMHelper)
