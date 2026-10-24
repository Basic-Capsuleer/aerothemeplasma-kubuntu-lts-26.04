/*
 *  SPDX-FileCopyrightText: 2026 catpswin56 <catpswin5@proton.me>
 *
 *  SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "aeroshellutilsplugin.h"

#include "sddm.h"

#include <QQmlEngine>

void AeroShellUtilsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<SDDM>(uri, 1, 0, "SDDM");
}

#include "moc_aeroshellutilsplugin.cpp"
