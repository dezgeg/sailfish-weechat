#pragma once

#include <QTcpSocket>
#include <QDataStream>
#include <memory>

typedef uint64_t WPointer;

class RelayConnection : public QObject {
Q_OBJECT

    std::unique_ptr<QTcpSocket> sock;
    QDataStream stream;
    qint64 remainingBytesInFrame;

    QByteArray readByteArray(uint32_t len);
    QByteArray readString();
    WPointer readPointer();
    QVariant readFieldOfType(QByteArray type);

public Q_SLOTS:
    bool readRelayReply();

Q_SIGNALS:
    void relayConnected();
    void relayMessageReceived(QByteArray frameId, QVariant message);

public:
    void writeString(const char* string);
    void flush() {
        sock->flush();
    }

    RelayConnection()
            : sock(std::unique_ptr<QTcpSocket>(new QTcpSocket())),
              stream(sock.get()),
              remainingBytesInFrame(0) {
        connect(sock.get(), &QTcpSocket::connected, [=]() {
            emit relayConnected();
        });
        connect(sock.get(), &QTcpSocket::readyRead, [=]() {
            while (readRelayReply())
                ;
        });
    }

    void connectToHost(const QString& hostname, qint16 port) {
        sock->connectToHost(hostname, port);
    }
};
