/*
 *  SPDX-FileCopyrightText: 2026 catpswin56 <catpswin5@proton.me>
 *
 *  SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef AEROSHELLUTILSPLUGIN_H
#define AEROSHELLUTILSPLUGIN_H

#include <QQmlExtensionPlugin>

class AeroShellUtilsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override;
};

#endif
