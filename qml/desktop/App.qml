import QtQuick 2.0

Rectangle {
    id: page

    SystemPalette { id: palette; colorGroup: SystemPalette.Active }

    Rectangle {
        id: channelListContainer

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 100
        color: palette.window

        ListView {
            anchors.fill: parent

            delegate: Text {
                text: model.modelData.shortName == '' ? model.modelData.fullName : model.modelData.shortName
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

                    text: model.modelData.prefix
                    color: 'white'
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

                    text: model.modelData.message
                    color: 'white'
                }
            }

            model: weechat.buffers[3].lines
        }
    }
}
