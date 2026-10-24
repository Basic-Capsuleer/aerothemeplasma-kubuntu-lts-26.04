/*
 *  SPDX-FileCopyrightText: 2026 catpswin56 <catpswin5@proton.me>
 *
 *  SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef AEROSHELLSDDM_H
#define AEROSHELLSDDM_H

#include <QObject>
#include <qqmlintegration.h>

#include <QFileSystemWatcher>

#include <KSharedConfig>

class SDDM : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

    Q_PROPERTY(QString currentTheme READ currentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY(QString currentBackground READ currentBackground NOTIFY currentBackgroundChanged)

public:
    explicit SDDM(QObject *parent = nullptr);
    ~SDDM() override;

    QString currentTheme();
    QString currentBackground();

Q_SIGNALS:
    void currentThemeChanged(QString previous);
    void currentBackgroundChanged(QString previous);

private Q_SLOTS:
    void reloadProperties();

private:
    KSharedConfig::Ptr m_cfg;
    QFileSystemWatcher *m_cfgWatcher;

    QString m_currentTheme{};

    QString m_currentBackground{};

};

#endif
