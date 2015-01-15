import QtQuick 2.0

Text {
    color: "#ffffff"
    font.pixelSize: 12
    onLinkActivated: Qt.openUrlExternally(link)
}
