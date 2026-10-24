// Qt
import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Window

// KDE

import org.kde.kwin as KWin
import org.kde.kwin.private.effects
import org.kde.kwindowsystem
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.kirigami as Kirigami

Item
{
    id: peekItem
    KWin.DBusCall {
        id: dbus
        service: "org.kde.plasmashell"
        path: "/PlasmaShell"
        dbusInterface: "org.freedesktop.DBus.Properties"
        method: "Get"
        arguments: ["org.kde.PlasmaShell", "editMode"]
        onFinished: (returnValue) => {
            peekItem.inEditMode = returnValue[0];
        }
    }
    property bool inEditMode: false;
    property bool compositingEnabled: (KWindowSystem.isPlatformX11 ? KX11Extras.compositingActive : true) && !inEditMode
    // On X11, show the frames whenever the desktop is shown. On Wayland a KWin script can't see that
    // state, so the show desktop button tells us when its hover peek starts and stops.
    property bool peekRequested: false
    property bool showingDesktop: KWindowSystem.isPlatformX11 ? KWindowSystem.showingDesktop : peekRequested

    KWin.ShortcutHandler {
        name: "SmodPeekShowFrames"
        text: "SMOD Peek: show window frames"
        sequence: ""
        onActivated: peekItem.peekRequested = true
    }
    KWin.ShortcutHandler {
        name: "SmodPeekHideFrames"
        text: "SMOD Peek: hide window frames"
        sequence: ""
        onActivated: peekItem.peekRequested = false
    }
    property bool instantiatorActive: true
    property bool delayActive: false

    Timer {
        id: timer
        interval: 50
        onTriggered: {
            delayActive = showingDesktop;
        }
    }
    onShowingDesktopChanged: {
        dbus.call();
        timer.start();
    }

    KWin.WindowFilterModel {
        id: windowmodel
        activity: KWin.Workspace.currentActivity
        desktop: KWin.Workspace.currentDesktop
        windowModel: KWin.WindowModel {}
        minimizedWindows: false
        windowType: ~KWin.WindowFilterModel.Dock &
        ~KWin.WindowFilterModel.Desktop &
        ~KWin.WindowFilterModel.Notification &
        ~KWin.WindowFilterModel.CriticalNotification
    }

    // Hack that ignores windows that are present in edit mode
    function ignoreWindow(w) {
        return !((w.resourceClass == "plasmashell" && w.skipTaskbar && w.caption == "plasmashell") ||
                 (w.resourceClass == "plasmashell") && w.dialog && !w.normalWindow && w.caption == "plasmashell" && w.modal && w.desktopFileName == "org.kde.plasmashell" && w.transient)
    }

    // One overlay per screen, the size of that screen. Everything is drawn per window from small,
    // ready-made images (no full-screen masks, no offscreen layers, no SVG rendering), because KWin
    // draws these windows on its main thread, so heavy drawing here made the whole desktop stutter.
    Instantiator
    {
        active: true
        model: KWin.Workspace.screens

        delegate: Window
        {
            id: overlay
            required property var modelData
            readonly property rect screenGeometry: modelData.geometry

            flags: Qt.BypassWindowManagerHint | Qt.FramelessWindowHint | Qt.WindowTransparentForInput
            // KWin on Wayland ignores WindowTransparentForInput for its own windows; without this the
            // overlay takes the mouse, the show desktop button loses hover and the peek flickers.
            property bool outputOnly: true
            // Marks this as an outline-type window, so KWin's window open animation (aeroglide)
            // leaves it alone.
            property bool __kwin_outline: true
            color: "transparent"
            x:       screenGeometry.x
            y:       screenGeometry.y
            width:   screenGeometry.width
            height:  screenGeometry.height
            visible: delayActive && compositingEnabled

            title: "aeropeek-aeroshell"

            function onThisScreen(w) {
                return w.x < screenGeometry.x + screenGeometry.width && w.x + w.width > screenGeometry.x &&
                       w.y < screenGeometry.y + screenGeometry.height && w.y + w.height > screenGeometry.y;
            }

            // Reflection: inside each window, the matching piece of this screen's reflection image
            Repeater
            {
                model: windowmodel
                delegate: Item
                {
                    x:      model.window.x - overlay.screenGeometry.x
                    y:      model.window.y - overlay.screenGeometry.y
                    width:  model.window.width
                    height: model.window.height
                    clip:   true
                    visible: !model.window.minimized && model.window.managed && ignoreWindow(model.window) && overlay.onThisScreen(model.window)

                    Image
                    {
                        x:          -parent.x
                        y:          -parent.y
                        width:      overlay.screenGeometry.width
                        height:     overlay.screenGeometry.height
                        source:     StandardPaths.locate(StandardPaths.GenericDataLocation, "/smod/kwin/reflections.png")
                        sourceSize: Qt.size(width, height)
                        smooth:     true
                    }
                }
            }

            // Paint frames
            Repeater
            {
                model: windowmodel
                delegate: Item
                {
                    id: peekwindow

                    // Assume the topmost window is the active window
                    property bool clientActive: model.window.stackingOrder == windowmodel.rowCount() - 1
                    // Assume window is maximized under these conditions
                    property bool clientMaximized: {

                        var windowFrame = model.window.frameGeometry;
                        var monitor = model.output.geometry;
                        if(monitor.top <= windowFrame.top && monitor.left <= windowFrame.left && monitor.bottom >= windowFrame.bottom && monitor.right >= windowFrame.right) {
                            return false;
                        }
                        var area = (windowFrame.width*windowFrame.height) / (monitor.width*monitor.height);
                        return area <= 1 && area > 0.9
                    }

                    anchors.fill: parent
                    visible: !model.window.minimized && !clientMaximized && model.window.managed && ignoreWindow(model.window) && overlay.onThisScreen(model.window)
                    z: model.window.stackingOrder

                    // Top corner glow, cut off at the window edge (the frame's rounded border is drawn on top)
                    Item {
                        id: glowCorners
                        anchors.fill: frame
                        clip: true

                        Image
                        {
                            id: leftCorner
                            property string path: "/smod/kwin/" + (peekwindow.clientActive ? "framecornereffect.png" : "framecornereffect-unfocus.png")
                            anchors.top: parent.top
                            anchors.left: parent.left
                            source:       StandardPaths.locate(StandardPaths.GenericDataLocation, path)
                            smooth:       true
                        }

                        Image
                        {
                            anchors.top: parent.top
                            anchors.right: parent.right
                            source:       leftCorner.source
                            smooth:       true
                            mirror:       true
                        }
                    }

                    // Frame shadow and frame: 9-slice images made from the pieces in peek-frame.svgz
                    // (shadow pieces: left 30, top 31, right 29, bottom 37 px; frame pieces: 9 px).
                    BorderImage
                    {
                        id: frameshadow
                        source: peekwindow.clientActive ? "res/peek-shadow.png" : "res/peek-shadowunfocus.png"
                        border { left: 30; top: 31; right: 29; bottom: 37 }
                        x:      frame.x - 14
                        y:      frame.y - 14
                        width:  frame.width  + 14 + 20
                        height: frame.height + 14 + 20
                    }

                    BorderImage
                    {
                        id: frame
                        source: peekwindow.clientActive ? "res/peek-frame.png" : "res/peek-unfocus.png"
                        border { left: 9; top: 9; right: 9; bottom: 9 }
                        x:      model.window.x - overlay.screenGeometry.x
                        y:      model.window.y - overlay.screenGeometry.y
                        width:  model.window.width
                        height: model.window.height
                    }
                }
            }
        }
    }
}
