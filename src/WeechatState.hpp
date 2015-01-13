#pragma once

#include <QDebug>
#include <QDateTime>
#include "RelayConnection.hpp"

#define PRINT_FIELD(field) #field ": " << that.field << ", "

class WeechatLine {
public:
    QDateTime timestamp;
    QByteArray prefix;
    QByteArray message;
    bool displayed;
};

QDebug operator<<(QDebug dbg, const WeechatLine& that) {
    dbg.nospace() << "Line{ " <<
            "timestamp: " << that.timestamp.toString("hh:mm:ss") << ", " <<
            PRINT_FIELD(displayed) <<
            PRINT_FIELD(prefix) <<
            PRINT_FIELD(message) << "}";
    return dbg.space();
}

class WeechatNick {
public:
    QByteArray prefix;
    QByteArray name;
};

QDebug operator<<(QDebug dbg, const WeechatNick& that) {
    dbg.nospace() << "Nick{ " <<
            PRINT_FIELD(prefix) <<
            PRINT_FIELD(name) << "}";
    return dbg.space();
}

class WeechatBuffer {
public:
    WPointer id;
    QByteArray fullName;
    QByteArray shortName;
    QByteArray title;
    QVariantHash localVariables;
    QList<WeechatLine> lines;
    QList<WeechatNick> nicks;
};

QDebug operator<<(QDebug dbg, const WeechatBuffer& that) {
    dbg.nospace() << "Buffer<" << that.id << ">{ " <<
            PRINT_FIELD(fullName) <<
            PRINT_FIELD(shortName) <<
            PRINT_FIELD(title) <<
            PRINT_FIELD(localVariables) << "}";
    return dbg.space();
}

class WeechatState {
public:
    RelayConnection relay;
    QHash<WPointer, WeechatBuffer> buffers;

    void process() {
        QVariant reply = relay.readReply();
        if (relay.currentFrameId == "listbuffers") {
            for (QVariant variant : reply.toList()) {
                QHash<QString, QVariant> hash = variant.toHash();

                WeechatBuffer buffer;
                buffer.id = hash["__path"].toList().at(0).toUInt();
                buffer.fullName = hash["full_name"].toByteArray();
                buffer.shortName = hash["short_name"].toByteArray();
                buffer.title = hash["title"].toByteArray();
                buffer.localVariables = hash["local_variables"].toHash();

                buffers[buffer.id] = std::move(buffer);
            }
        } else if (relay.currentFrameId == "listlines") {
            for (QVariant variant : reply.toList()) {
                QHash<QString, QVariant> hash = variant.toHash();
                WPointer bufferId = hash["__path"].toList().at(0).toUInt();

                WeechatLine line;
                line.timestamp = QDateTime::fromTime_t(hash["date"].toUInt());
                line.message = hash["message"].toByteArray();
                line.prefix = hash["prefix"].toByteArray();
                line.displayed = hash["displayed"].toBool();

                buffers[bufferId].lines.push_back(line);
            }
        } else if (relay.currentFrameId == "nicklist") {
            for (QVariant variant : reply.toList()) {
                QHash<QString, QVariant> hash = variant.toHash();
                if (hash["level"].toUInt() != 0) {
                    // not a real nick, but a nick group
                    continue;
                }

                WPointer bufferId = hash["__path"].toList().at(0).toUInt();

                WeechatNick nick;
                nick.name = hash["name"].toByteArray();
                nick.prefix = hash["prefix"].toByteArray();

                buffers[bufferId].nicks.push_back(nick);
            }
        }
    }
};
