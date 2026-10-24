/*
 *  SPDX-FileCopyrightText: 2026 catpswin56 <catpswin5@proton.me>
 *
 *  SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "sddm.h"

#include <QStandardPaths>
#include <QFile>

#include <KConfigGroup>

static const QString s_cfgPath{"/etc/sddm.conf.d/kde_settings.conf"};

SDDM::SDDM(QObject *parent)
    : QObject(parent)
    , m_cfg{KSharedConfig::openConfig(s_cfgPath, KConfig::SimpleConfig)}
    , m_cfgWatcher{new QFileSystemWatcher({s_cfgPath}, this)}
{
    reloadProperties();
    connect(m_cfgWatcher, &QFileSystemWatcher::fileChanged, this, &SDDM::reloadProperties);
}

SDDM::~SDDM() = default;


QString SDDM::currentTheme()
{ return m_currentTheme; }

QString SDDM::currentBackground()
{ return m_currentBackground; }

// this is a mess lmfao
// TODO: clean this mess of a code
void SDDM::reloadProperties()
{
    QString prev{};
    KConfigGroup cg;

    m_cfg->reparseConfiguration();

    {
        cg = m_cfg->group("Theme");
        prev = m_currentTheme;
        m_currentTheme = cg.readEntry("Current", "sddm-theme-mod");
        Q_EMIT currentThemeChanged(prev);
    }

    {
        QStringList paths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "sddm", QStandardPaths::LocateDirectory);

        if(!paths.isEmpty()) {
            QString path = paths.last();

            if(!(path.startsWith("/home") || path.startsWith("~/"))) {
                path += "/themes/" + m_currentTheme + "/";

                if(QFile::exists(path + "background")) {
                    prev = m_currentBackground;
                    m_currentBackground = path + "background";
                    Q_EMIT currentBackgroundChanged(prev);

                } else if(QFile::exists(path + "default-background")) {
                    prev = m_currentBackground;
                    m_currentBackground = path + "default-background";
                    Q_EMIT currentBackgroundChanged(prev);

                } else { // non-aeroshell sddm themes
                    KSharedConfig::Ptr conf = KSharedConfig::openConfig(path + "theme.conf.user");
                    if(conf) {
                        conf->addConfigSources({path + "theme.conf"});
                        cg = conf->group("General");

                        QString bg = cg.readEntry("background", "");

                        prev = m_currentBackground;
                        if(bg.startsWith("./") || !bg.startsWith("/")) {
                            m_currentBackground = path + bg;
                        } else {
                            m_currentBackground = bg;
                        }
                        Q_EMIT currentBackgroundChanged(prev);
                    }
                }
            }
        }
    }
}

#include "moc_sddm.cpp"
