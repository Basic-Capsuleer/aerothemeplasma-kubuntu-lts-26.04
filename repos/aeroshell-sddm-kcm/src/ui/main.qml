import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

KCM.SimpleKCM {
    id: root

    property alias themeToConfigure: comboBox.currentValue

    property alias enableStartup: enableStartup.checked
    onEnableStartupChanged: changed();

    property alias playSound: playSound.checked
    onPlaySoundChanged: changed();

    property alias forceUserSelect: forceUserSelect.checked
    onForceUserSelectChanged: changed();

    property alias rdpBackground: rdpBackground.checked
    onRdpBackgroundChanged: changed();

    property alias backgroundSrc: background.source
    onBackgroundSrcChanged: changed();

    signal changed()

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    KCM.ConfigModule.buttons: KCM.ConfigModule.Apply

    component CustomGroupBox: QQC2.GroupBox {
        id: gbox
        label: QQC2.Label {
            id: lbl
            x: gbox.leftPadding + 2
            y: lbl.implicitHeight/2-gbox.bottomPadding-1
            width: lbl.implicitWidth
            text: gbox.title
            elide: Text.ElideRight
            Rectangle {
                anchors.fill: parent
                anchors.leftMargin: -2
                anchors.rightMargin: -2
                color: Kirigami.Theme.backgroundColor
                z: -1
            }
        }
        background: Rectangle {
            y: gbox.topPadding - gbox.bottomPadding*2
            width: parent.width
            height: parent.height - gbox.topPadding + gbox.bottomPadding*2
            color: "transparent"
            border.color: "#d5dfe5"
            radius: 3
        }
    }

    FileDialog {
        id: fileDlg

        onAccepted: backgroundSrc = selectedFile
        nameFilters: [i18n("PNG files (*.png)"), i18n("JPEG files (*.jpg *.jpeg)")]
    }

    footer: Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: 1

        color: "black"

        opacity: 0.25
    }

    ColumnLayout {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top

            margins: Kirigami.Units.largeSpacing
        }

        RowLayout {
            Text { text: i18n("Theme to configure:") }
            QQC2.ComboBox {
                id: comboBox
                textRole: "display"
                valueRole: "path"
                model: kcm.themesModel
            }
        }

        CustomGroupBox {
            Layout.fillWidth: true

            title: i18n("General")

            ColumnLayout {
                anchors.fill: parent

                QQC2.CheckBox {
                    id: enableStartup
                    text: i18n("Enable Vista Startup Pearl Animation")
                }

                QQC2.CheckBox {
                    id: playSound
                    text: i18n("Play Startup sound")
                }

                QQC2.CheckBox {
                    id: forceUserSelect
                    text: i18n("Force user selection")
                }

                QQC2.CheckBox {
                    id: rdpBackground
                    text: i18n("Use RDP background")
                }
            }
        }

        CustomGroupBox {
            Layout.fillWidth: true

            Layout.minimumHeight: 395
            Layout.maximumHeight: 395

            title: i18n("Background")
            enabled: !rdpBackground.checked

            ColumnLayout {
                anchors.fill: parent

                spacing: 2

                Image {
                    id: background

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    fillMode: Image.PreserveAspectFit

                    visible: enabled
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    color: "#1D5F7A"

                    visible: !enabled
                }

                RowLayout {
                    QQC2.Button {
                        id: changeBackground

                        Layout.fillWidth: true

                        text: i18n("Change...")
                        onClicked: fileDlg.open();
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
