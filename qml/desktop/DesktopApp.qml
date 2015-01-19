import QtQuick 2.0
import QtQuick.Controls 1.3
import ".."

Rectangle {
    Action { shortcut: "Alt+Left"; onTriggered: ircPage.switchToPrevBuffer() }
    Action { shortcut: "Alt+Right"; onTriggered: ircPage.switchToNextBuffer() }
    Action { shortcut: "Alt+1"; onTriggered: ircPage.switchToBuffer(0) }
    Action { shortcut: "Alt+2"; onTriggered: ircPage.switchToBuffer(1) }
    Action { shortcut: "Alt+3"; onTriggered: ircPage.switchToBuffer(2) }
    Action { shortcut: "Alt+4"; onTriggered: ircPage.switchToBuffer(3) }
    Action { shortcut: "Alt+5"; onTriggered: ircPage.switchToBuffer(4) }
    Action { shortcut: "Alt+6"; onTriggered: ircPage.switchToBuffer(5) }
    Action { shortcut: "Alt+7"; onTriggered: ircPage.switchToBuffer(6) }
    Action { shortcut: "Alt+8"; onTriggered: ircPage.switchToBuffer(7) }
    Action { shortcut: "Alt+9"; onTriggered: ircPage.switchToBuffer(8) }
    Action { shortcut: "Alt+0"; onTriggered: ircPage.switchToBuffer(9) }

    Action { shortcut: "Page Up"; onTriggered: ircPage.scrollLines(-1) }
    Action { shortcut: "Page Down"; onTriggered: ircPage.scrollLines(1) }

    IrcPage {
        id: ircPage
        anchors.fill: parent
    }
}
