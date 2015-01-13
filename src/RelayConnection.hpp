#pragma once

#include <QTcpSocket>
#include <memory>

typedef uint64_t WPointer;

class RelayConnection {
    std::unique_ptr<QTcpSocket> sock;
    QDataStream stream;
    qint64 remainingBytesInFrame;

    QByteArray readByteArray(uint32_t len);
    QByteArray readString();
    WPointer readPointer();
    QVariant readFieldOfType(QByteArray type);

public:
    QByteArray currentFrameId; // FIXME
    QVariant readReply();

    void writeString(const char* string);
    void flush() {
        sock->flush();
    }

    RelayConnection()
            : sock(std::unique_ptr<QTcpSocket>(new QTcpSocket())),
              stream(sock.get()),
              remainingBytesInFrame(0) { }

    void connect(const QString& hostname, qint16 port) {
        sock->connectToHost(hostname, port);
        sock->waitForConnected();
    }
};
