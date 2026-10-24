/*
    SPDX-FileCopyrightText: 2013-2015 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami
import org.kde.ksvg as KSvg

Item {
    id: root
    property Item toolTip
    property int preferredTextWidth: Kirigami.Units.gridUnit * 10

    implicitWidth: mainLayout.implicitWidth
    implicitHeight: mainLayout.implicitHeight

    LayoutMirroring.enabled: Application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    RowLayout {
        id: mainLayout
        anchors.centerIn: parent

        Image {
            source: root.toolTip ? root.toolTip.image : ""
            visible: root.toolTip !== null && root.toolTip.image !== ""
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Kirigami.Icon {
            animated: false
            source: root.toolTip ? root.toolTip.icon : ""
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            visible: root.toolTip !== null && root.toolTip.icon !== "" && root.toolTip.image === "" && valid
            Layout.preferredWidth: Kirigami.Units.iconSizes.medium
            Layout.preferredHeight: Kirigami.Units.iconSizes.medium
            Layout.leftMargin: Kirigami.Units.smallSpacing*2
        }

        ColumnLayout {
            Layout.maximumWidth: root.preferredTextWidth
            spacing: 0

            Kirigami.Heading {
                level: 5
                Layout.fillWidth: true
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                text: root.toolTip ? root.toolTip.mainText : ""
                textFormat: Text.PlainText
                visible: text !== ""
                opacity: 0.75
            }

            PlasmaComponents.Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: root.toolTip ? root.toolTip.subText : ""
                textFormat: root.toolTip ? root.toolTip.textFormat : Text.AutoText
                opacity: 0.75
                visible: text !== ""
                maximumLineCount: 8
            }
        }
    }
}
