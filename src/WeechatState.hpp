#pragma once

#include <algorithm>
#include <QDebug>
#include <QDateTime>
#include <QObject>
#include <QQmlListProperty>
#include "RelayConnection.hpp"

#define PRINT_FIELD(field) #field ": " << that.field << ", "
#define PROP(type, field) type field; Q_PROPERTY(type field MEMBER field CONSTANT)

class WeechatLine : public QObject {
Q_OBJECT
public:
    explicit WeechatLine(QObject* parent = nullptr) : QObject(parent) { }

    PROP(QDateTime, timestamp);
    PROP(QByteArray, prefix);
    PROP(QByteArray, message);
    PROP(bool, displayed);
};

inline QDebug operator<<(QDebug dbg, const WeechatLine& that) {
    dbg.nospace() << "Line{ " <<
            "timestamp: " << that.timestamp.toString("hh:mm:ss") << ", " <<
            PRINT_FIELD(displayed) <<
            PRINT_FIELD(prefix) <<
            PRINT_FIELD(message) << "}";
    return dbg.space();
}

class WeechatNick : public QObject {
Q_OBJECT
public:
    explicit WeechatNick(QObject* parent = nullptr) : QObject(parent) { }

    PROP(QByteArray, prefix);
    PROP(QByteArray, name);
};

inline QDebug operator<<(QDebug dbg, const WeechatNick& that) {
    dbg.nospace() << "Nick{ " <<
            PRINT_FIELD(prefix) <<
            PRINT_FIELD(name) << "}";
    return dbg.space();
}

class WeechatBuffer : public QObject {
Q_OBJECT
public:
    explicit WeechatBuffer(QObject* parent = nullptr) : QObject(parent) { }

    PROP(WPointer, id);
    PROP(QByteArray, fullName);
    PROP(QByteArray, shortName);
    PROP(QByteArray, title);
    PROP(QVariantHash, localVariables);

    QList<WeechatLine*> lines;
    QList<WeechatNick*> nicks;

    Q_PROPERTY(QQmlListProperty<WeechatLine> lines READ _getLines CONSTANT);
    Q_PROPERTY(QQmlListProperty<WeechatNick> nicks READ _getNicks CONSTANT);

    QQmlListProperty<WeechatLine> _getLines() {
        return QQmlListProperty<WeechatLine>(this, lines);
    }

    QQmlListProperty<WeechatNick> _getNicks() {
        return QQmlListProperty<WeechatNick>(this, nicks);
    }
};

inline QDebug operator<<(QDebug dbg, const WeechatBuffer& that) {
    dbg.nospace() << "Buffer<" << that.id << ">{ " <<
            PRINT_FIELD(fullName) <<
            PRINT_FIELD(shortName) <<
            PRINT_FIELD(title) <<
            PRINT_FIELD(localVariables) << "}";
    return dbg.space();
}

class WeechatState : public QObject {
Q_OBJECT
public:
    explicit WeechatState(QObject* parent = nullptr) : QObject(parent) { }

    RelayConnection relay;
    QHash<WPointer, WeechatBuffer*> buffers;
    QList<WeechatBuffer*> buffersInOrder;

    Q_PROPERTY(QQmlListProperty<WeechatBuffer> buffers READ _getBuffersInOrder CONSTANT);

    QQmlListProperty<WeechatBuffer> _getBuffersInOrder() {
        return QQmlListProperty<WeechatBuffer>(this, buffersInOrder);
    }

    void process() {
        QVariant reply = relay.readReply();
        if (relay.currentFrameId == "listbuffers") {
            for (QVariant variant : reply.toList()) {
                QHash<QString, QVariant> hash = variant.toHash();

                WeechatBuffer* buffer = new WeechatBuffer();
                buffer->id = hash["__path"].toList().at(0).toUInt();
                buffer->fullName = hash["full_name"].toByteArray();
                buffer->shortName = hash["short_name"].toByteArray();
                buffer->title = hash["title"].toByteArray();
                buffer->localVariables = hash["local_variables"].toHash();

                buffers[buffer->id] = buffer;
                buffersInOrder.push_back(buffer);
            }
        } else if (relay.currentFrameId == "listlines") {
            for (QVariant variant : reply.toList()) {
                QHash<QString, QVariant> hash = variant.toHash();
                WPointer bufferId = hash["__path"].toList().at(0).toUInt();

                WeechatLine* line = new WeechatLine(buffers[bufferId]);
                line->timestamp = QDateTime::fromTime_t(hash["date"].toUInt());
                line->message = hash["message"].toByteArray();
                line->prefix = hash["prefix"].toByteArray();
                line->displayed = hash["displayed"].toBool();

                buffers[bufferId]->lines.push_back(line);
            }
            for (WeechatBuffer* buf : buffers) {
                std::reverse(std::begin(buf->lines), std::end(buf->lines));
            }
        } else if (relay.currentFrameId == "nicklist") {
            for (QVariant variant : reply.toList()) {
                QHash<QString, QVariant> hash = variant.toHash();
                if (hash["level"].toUInt() != 0) {
                    // not a real nick, but a nick group
                    continue;
                }

                WPointer bufferId = hash["__path"].toList().at(0).toUInt();

                WeechatNick* nick = new WeechatNick(buffers[bufferId]);
                nick->name = hash["name"].toByteArray();
                nick->prefix = hash["prefix"].toByteArray();

                buffers[bufferId]->nicks.push_back(nick);
            }
        }
    }
};
