import QtQuick 2.0
import QtQuick.Controls 1.3
import SailfishWeechat 1.0

Rectangle {
    id: page

    Action { shortcut: "Alt+Left"; onTriggered: channelListView.decrementCurrentIndex() }
    Action { shortcut: "Alt+Right"; onTriggered: channelListView.incrementCurrentIndex() }
    Action { shortcut: "Alt+1"; onTriggered: channelListView.currentIndex = 0 }
    Action { shortcut: "Alt+2"; onTriggered: channelListView.currentIndex = 1 }
    Action { shortcut: "Alt+3"; onTriggered: channelListView.currentIndex = 2 }
    Action { shortcut: "Alt+4"; onTriggered: channelListView.currentIndex = 3 }
    Action { shortcut: "Alt+5"; onTriggered: channelListView.currentIndex = 4 }
    Action { shortcut: "Alt+6"; onTriggered: channelListView.currentIndex = 5 }
    Action { shortcut: "Alt+7"; onTriggered: channelListView.currentIndex = 6 }
    Action { shortcut: "Alt+8"; onTriggered: channelListView.currentIndex = 7 }
    Action { shortcut: "Alt+9"; onTriggered: channelListView.currentIndex = 8 }
    Action { shortcut: "Alt+0"; onTriggered: channelListView.currentIndex = 9 }

    Action { shortcut: "Page Up"; onTriggered: bufferContainer.scrollLines(-1) }
    Action { shortcut: "Page Down"; onTriggered: bufferContainer.scrollLines(1) }

    property int textMargin: 3
    SystemPalette { id: palette; colorGroup: SystemPalette.Active }
    FontMeasurer { id: fontMeasurer }
    StyledText { id: dummyText; visible: false; text: "FOObar" }
    StyledText {
        id: dummyMonospaceText
        visible: false
        text: "FOObar"
        font.family: 'monospace'
        font.bold: true
    }

    Rectangle {
        id: channelListContainer

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: measureBufferNames()
        color: 'darkgray'

        property int numberWidth: fontMeasurer.findMaxWidth(dummyMonospaceText.font, ['88'])

        function measureBufferNames() {
            var arr = [];
            for (var i = 0; i < weechat.buffers.length; i++)
                arr.push(bufferNameHtml(weechat.buffers[i]));
            return fontMeasurer.findMaxWidth(dummyText.font, arr) + textMargin + numberWidth
        }

        function bufferNameHtml(buffer) {
            return buffer.shortName == '' ? buffer.fullName : buffer.shortName;
        }

        ListView {
            id: channelListView
            anchors.fill: parent

            delegate: Rectangle {
                id: channelContainer
                height: bufferName.height
                width: parent.width

                color: ListView.isCurrentItem ? 'white' : channelListContainer.color
                property color textColor: 'black'

                StyledText {
                    id: bufferNumber
                    color: channelContainer.textColor
                    textFormat: Text.StyledText
                    text: model.modelData.number + (model.modelData.number < 10 ? ' ' : '')
                    font.family: 'monospace'
                    font.bold: true
                    width: channelListContainer.numberWidth
                }

                StyledText {
                    id: bufferName
                    anchors.left: bufferNumber.right
                    anchors.leftMargin: textMargin

                    color: channelContainer.textColor
                    textFormat: Text.StyledText
                    text: channelListContainer.bufferNameHtml(model.modelData)
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        channelListView.currentIndex = index
                    }
                }
            }

            model: weechat.buffers
            onCurrentIndexChanged: bufferContainer.changeVisibleBuffer()
            keyNavigationWraps: true
        }
    }

    Rectangle {
        id: titleContainer

        anchors.top: parent.top
        anchors.left: channelListContainer.right
        anchors.right: parent.right

        height: titleText.height
        color: 'blue'
        z: 1

        StyledText {
            id: titleText
            anchors.left: parent.left
            anchors.leftMargin: textMargin
            text: weechat.buffers[channelListView.currentIndex].title
        }
    }

    ScrollBar {
        id: scrollBar
        anchors.top: titleContainer.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 12

        orientation: Qt.Vertical
        position: bufferContainer.currentVisibleBuffer.visibleArea.yPosition
        pageSize: bufferContainer.currentVisibleBuffer.visibleArea.heightRatio
    }

    Rectangle {
        id: bufferContainer

        anchors.left: channelListContainer.right
        anchors.right: scrollBar.left
        anchors.top: titleContainer.bottom
        anchors.bottom: parent.bottom

        color: 'black'

        property variant bufferItems: []
        property variant currentVisibleBuffer: null

        function changeVisibleBuffer() {
            var i = channelListView.currentIndex
            if (i < bufferItems.length) {
                if (currentVisibleBuffer)
                    currentVisibleBuffer.visible = false
                currentVisibleBuffer = bufferItems[i]
                currentVisibleBuffer.visible = true
            }
        }

        function scrollLines(dir) {
            currentVisibleBuffer.contentY += dir * currentVisibleBuffer.height * 0.5
            currentVisibleBuffer.returnToBounds()
        }

        Component.onCompleted: {
            for (var i = 0; i < weechat.buffers.length; i++) {
                var weechatBuffer = weechat.buffers[i];
                bufferItems[i] = bufferComponent.createObject(bufferContainer, { bufferIndex: i, visible: false })
            }
            changeVisibleBuffer()
        }
    }

    Component {
        id: bufferComponent

        ListView {
            id: bufferListView
            anchors.fill: parent

            function measureNicks() {
                var arr = [];
                var nicks = weechat.buffers[bufferIndex].nicks;
                for (var i = 0; i < nicks.length; i++)
                    arr.push(nicks[i].name);
                var r = fontMeasurer.findMaxWidth(dummyText.font, arr) + textMargin
                // console.log('measureNicks', arr, r);
                return r;
            }

            property int bufferIndex: -1
            property int nickWidth: measureNicks()
            property int numTextLinesPerScreen: Math.round(width / fontMeasurer.getFontHeight())

            Component.onCompleted: positionViewAtEnd()

            delegate: Item {
                height: messageLabel.height
                anchors.left: parent.left
                anchors.right: parent.right

                StyledText {
                    id: timestampLabel

                    text: model.modelData.formatTimestamp()
                    font.family: 'monospace'
                    width: fontMeasurer.findMaxWidth(timestampLabel.font, ['88:88:88'])
                }

                StyledText {
                    id: nickLabel
                    anchors.leftMargin: textMargin
                    anchors.left: timestampLabel.right
                    width: nickWidth

                    textFormat: Text.StyledText
                    horizontalAlignment: Text.AlignRight

                    text: model.modelData.prefix
                }

                Rectangle {
                    id: border
                    anchors.left: nickLabel.right
                    anchors.leftMargin: textMargin
                    height: parent.height
                    width: 1
                }

                StyledText {
                    id: messageLabel
                    anchors.left: border.right
                    anchors.right: parent.right
                    anchors.leftMargin: 5

                    wrapMode: Text.Wrap
                    textFormat: Text.StyledText

                    text: model.modelData.message
                }
            }

            snapMode: ListView.SnapToItem
            model: weechat.buffers[bufferIndex].lines
        }
    }
}
