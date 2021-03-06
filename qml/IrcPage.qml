import QtQuick 2.0
import SailfishWeechat 1.0
import Sailfish.Silica 1.0

Page {
    id: page

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

    function switchToBuffer(num) {
        channelListView.currentIndex = num - 1
    }
    function switchToPrevBuffer() {
        channelListView.decrementCurrentIndex()
    }
    function switchToNextBuffer() {
        channelListView.incrementCurrentIndex()
    }
    function scrollLines(delta) {
        bufferContainer.scrollLines(delta)
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

        listView: bufferContainer
    }

    Rectangle {
        id: bufferContainer

        anchors.left: channelListContainer.right
        anchors.right: scrollBar.left
        anchors.top: titleContainer.bottom
        anchors.bottom: parent.bottom

        color: 'black'

        property var bufferItems: []
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

            Component.onCompleted: {
                positionViewAtEnd()
                weechat.buffers[bufferIndex].rowsInserted.connect(function() {
                    if (bufferListView.atYEnd)
                        bufferListView.positionViewAtEnd()
                })
            }

            delegate: Item {
                height: messageLabel.height
                anchors.left: parent.left
                anchors.right: parent.right

                StyledText {
                    id: timestampLabel

                    text: model.timestamp.toString().substring(11, 11+8) // HACK
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

                    text: model.prefix
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

                    text: model.message
                }
            }

            snapMode: ListView.SnapToItem
            model: weechat.buffers[bufferIndex]
        }
    }
}
