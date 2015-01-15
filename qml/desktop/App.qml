import QtQuick 2.0

Rectangle {
    id: page

    SystemPalette { id: palette; colorGroup: SystemPalette.Active }

    Rectangle {
        id: channelListContainer

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 100
        color: 'darkgray'

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
        }
    }

    Rectangle {
        anchors.left: channelListContainer.right
        anchors.top: parent.top;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        color: 'black'

        ListView {
            anchors.fill: parent
            anchors.leftMargin: 3

            Component.onCompleted: positionViewAtEnd()

            delegate: Item {
                height: messageLabel.height
                anchors.left: parent.left
                anchors.right: parent.right

                Text {
                    id: timestampLabel

                    text: '12:34:56'
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

            model: weechat.buffers[channelListView.currentIndex].lines
        }
    }
}
