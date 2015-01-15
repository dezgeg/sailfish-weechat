import QtQuick 2.0
import SailfishWeechat 1.0

Rectangle {
    id: page

    property int textMargin: 3
    SystemPalette { id: palette; colorGroup: SystemPalette.Active }
    FontMeasurer { id: fontMeasurer }

    Rectangle {
        id: channelListContainer

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: measureBufferNames()
        color: 'darkgray'

        function measureBufferNames() {
            var arr = [];
            for (var i = 0; i < weechat.buffers.length; i++)
                arr.push(weechat.buffers[i].shortName);
            return fontMeasurer.findMaxWidth(arr) + textMargin
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

                Text {
                    id: bufferName
                    text: model.modelData.shortName == '' ? model.modelData.fullName : model.modelData.shortName
                    color: channelContainer.textColor
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

        Text {
            id: titleText
            anchors.left: parent.left
            anchors.leftMargin: textMargin
            text: weechat.buffers[channelListView.currentIndex].title
            color: 'white'
        }
    }

    Rectangle {
        id: bufferContainer

        anchors.left: channelListContainer.right
        anchors.right: parent.right
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
            anchors.fill: parent
            anchors.leftMargin: textMargin

            property variant bufferIndex: -1
            Component.onCompleted: positionViewAtEnd()

            delegate: Item {
                height: messageLabel.height
                anchors.left: parent.left
                anchors.right: parent.right

                Text {
                    id: timestampLabel

                    text: model.modelData.formatTimestamp()
                    font.family: 'monospace'
                    color: 'white'
                }

                Text {
                    id: nickLabel
                    anchors.leftMargin: 5
                    anchors.left: timestampLabel.right

                    textFormat: Text.RichText

                    text: model.modelData.prefix
                }

                Rectangle {
                    id: border
                    anchors.left: nickLabel.right
                    anchors.leftMargin: 5
                    height: parent.height
                    width: 1
                }

                Text {
                    id: messageLabel
                    anchors.left: border.right
                    anchors.right: parent.right
                    anchors.leftMargin: 5

                    wrapMode: Text.Wrap
                    textFormat: Text.RichText

                    text: model.modelData.message
                }
            }

            model: weechat.buffers[bufferIndex].lines
        }
    }
}
