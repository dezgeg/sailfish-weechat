#include "WeechatState.hpp"
#include "TerminalColors.hpp"

#define PRINT_FIELD(field) #field ": " << that.field << ", "

QDebug operator<<(QDebug dbg, const WeechatLine& that) {
    dbg.nospace() << "Line{ " <<
            "timestamp: " << that.formatTimestamp() << ", " <<
            PRINT_FIELD(displayed) <<
            PRINT_FIELD(prefix) <<
            PRINT_FIELD(message) << "}";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const WeechatNick& that) {
    dbg.nospace() << "Nick{ " <<
            PRINT_FIELD(prefix) <<
            PRINT_FIELD(name) << "}";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const WeechatBuffer& that) {
    dbg.nospace() << "Buffer<" << that.id << ">{ " <<
            PRINT_FIELD(fullName) <<
            PRINT_FIELD(shortName) <<
            PRINT_FIELD(title) <<
            PRINT_FIELD(localVariables) << "}";
    return dbg.space();
}

void WeechatState::process(QByteArray frameId, QVariant reply) {
    if (frameId == "listbuffers") {
        for (QVariant variant : reply.toList()) {
            QHash<QString, QVariant> hash = variant.toHash();

            WeechatBuffer* buffer = new WeechatBuffer();
            buffer->id = hash["__path"].toList().at(0).toUInt();
            buffer->fullName = hash["full_name"].toByteArray();
            buffer->shortName = hash["short_name"].toByteArray();
            buffer->title = "<html><font color=\"#ffffff\">" + convertUrls(hash["title"].toByteArray()) + "</font></html>";
            buffer->localVariables = hash["local_variables"].toHash();
            buffer->number = hash["number"].toInt();

            buffers[buffer->id] = buffer;
            buffersInOrder.push_back(buffer);
        }
    } else if (frameId == "listlines") {
        for (QVariant variant : reply.toList()) {
            QHash<QString, QVariant> hash = variant.toHash();
            WPointer bufferId = hash["__path"].toList().at(0).toUInt();

            WeechatLine* line = new WeechatLine(buffers[bufferId]);
            line->timestamp = QDateTime::fromTime_t(hash["date"].toUInt());
            line->message = convertUrls(convertColorCodes(hash["message"].toByteArray()));
            line->prefix = convertColorCodes(hash["prefix"].toByteArray());
            line->displayed = hash["displayed"].toBool();

            buffers[bufferId]->lines.push_back(line);
        }
        for (WeechatBuffer* buf : buffers) {
            std::reverse(std::begin(buf->lines), std::end(buf->lines));
        }
    } else if (frameId == "nicklist") {
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
        // initial server state is now synced after nicklist is received
        emit connectionEstablished();
    }
}
