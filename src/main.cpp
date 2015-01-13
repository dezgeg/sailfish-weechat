#include <QApplication>
#include <QTcpSocket>
#include <memory>
#include <string.h>
#include <stdint.h>

static const int TYPE_LENGTH = 3;

qint64 remainingBytesInFrame;
static QDataStream stream;
std::unique_ptr<QTcpSocket> sock;

static inline void writeString(const char* string) {
    stream.writeRawData(string, (int)strlen(string));
}

QByteArray readByteArray(uint32_t len) {
    QByteArray ret;
    ret.resize(len);
    stream.readRawData(ret.data(), len);
    return ret;
}

QByteArray readString() {
    uint32_t len;
    stream >> len;
    if (len == UINT32_MAX || len == 0) {
        return QByteArray();
    }
    return readByteArray(len);
}

uint64_t readPointer() {
    uint8_t len;
    stream >> len;
    QByteArray temp = readByteArray(len);
    return temp.toUInt(nullptr, 16);
}

QVariant readFieldOfType(QByteArray type) {
    if (type == QByteArray("inf")) {
        QByteArray key = readString();
        QByteArray value = readString();
        QList<QVariant> list = { key, value };
        return QVariant(list);
    } else if (type == QByteArray("chr")) {
        uint8_t b;
        stream >> b;
        return QVariant(b);
    } else if (type == QByteArray("int")) {
        uint32_t i;
        stream >> i;
        return QVariant(i);
    } else if (type == QByteArray("tim") || type == QByteArray("lon")) {
        uint8_t len;
        stream >> len;
        QByteArray data = readByteArray(len);
        return QVariant(data.toInt(nullptr, 10));
    } else if (type == QByteArray("ptr")) {
        return QVariant(qulonglong(readPointer()));
    } else if (type == QByteArray("str") || type == QByteArray("buf")) {
        return readString();
    } else if (type == QByteArray("arr")) {
        QByteArray elementType = readByteArray(TYPE_LENGTH);
        uint32_t count;
        stream >> count;
        QVariantList retval;
        for (uint32_t i = 0; i < count; i++) {
            retval.push_back(readFieldOfType(elementType));
        }
        return retval;
    } else if (type == QByteArray("htb")) {
        QByteArray keyType = readByteArray(TYPE_LENGTH);
        QByteArray valueType = readByteArray(TYPE_LENGTH);
        Q_ASSERT(keyType == QByteArray("str")); // XXX - QVariant only supports this

        uint32_t count;
        stream >> count;
        QVariantHash retval;
        for (uint32_t i = 0; i < count; i++) {
            QVariant key = readFieldOfType(keyType);
            QVariant value = readFieldOfType(valueType);
            retval[key.toString()] = value;
        }
        return QVariant(retval);
    } else if (type == QByteArray("hda")) {
        QByteArray path = readString();
        // number of '/':s in path is needed to decode the p-paths
        int numPointers = 1;
        for (char c : path) {
            if (c == '/') {
                numPointers++;
            }
        }

        QByteArray keys = readString();
        QList<QByteArray> fields;
        QList<QByteArray> types;
        for (QByteArray ba : keys.split(',')) {
            QList<QByteArray> pair = ba.split(':');
            fields.push_back(pair[0]);
            types.push_back(pair[1]);
        }

        uint32_t count;
        stream >> count;

        QList<QVariant> retval;
        for (uint32_t i = 0; i < count; i++) {
            QVariantList pointers;
            for (int j = 0; j < numPointers; j++) {
                pointers.push_back(QVariant((qulonglong)readPointer()));
            }
            QVariantHash element;
            element["__path"] = pointers;

            for (int j = 0; j < fields.length(); j++) {
                element[fields[j]] = readFieldOfType(types[j]);
            }
            retval.push_back(element);
        }
        return retval;
    }

    qDebug() << "Unknown data of type:" << type;
    return QVariant();
}

QVariant readReply() {
    qint64 posAtStart = sock->pos();

    if (remainingBytesInFrame == 0) {
        while (sock->bytesAvailable() < 4) {
            sock->waitForReadyRead();
        }
        uint32_t frameLength;
        stream >> frameLength;

        while (sock->bytesAvailable() < frameLength - 4) {
            sock->waitForReadyRead();
        }

        uint8_t compressionFlag;
        stream >> compressionFlag;
        QString id = readString();
        qDebug() << "Frame - compression:" << compressionFlag << "id:" << id;
        remainingBytesInFrame = frameLength;
    }

    QByteArray type = readByteArray(TYPE_LENGTH);
    QVariant ret = readFieldOfType(type);
    remainingBytesInFrame -= sock->pos() - posAtStart;
    return ret;
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    sock = std::unique_ptr<QTcpSocket>(new QTcpSocket());
    sock->connectToHost("localhost", 9001);
    if (!sock->waitForConnected()) {
        return 1;
    }
    qDebug() << "Connected.";
    stream.setDevice(sock.get());

    writeString("init password=ffc54fe75cb3db7499544dfce520fd65589045756dd94d3bdb6bb38443ed7e61,compression=off\n");
    writeString("(listbuffers) hdata buffer:gui_buffers(*) number,full_name,short_name,type,nicklist,title,local_variables\n");
    writeString("(listlines) hdata buffer:gui_buffers(*)/own_lines/last_line(-50)/data date,displayed,prefix,message\n");
    writeString("(nicklist) nicklist\n");

//    writeString("info version\n");
//    writeString("ping foo\n");
//    writeString("hdata buffer:gui_buffers(*)\n");
//    writeString("nicklist\n");
//    writeString("test\ntest\n");
    sock->flush();

    for (int i = 0; i < 3; i++) {
        QVariant reply = readReply();
        if (reply.type() == QVariant::List) {
            for (QVariant& v : reply.toList()) {
                qDebug() << "    " << v;
            }
        } else {
            qDebug() << reply;
        }
    }
}
