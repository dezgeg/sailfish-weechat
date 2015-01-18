#pragma once

#include <algorithm>
#include <QAbstractListModel>
#include <QDebug>
#include <QDateTime>
#include <QObject>
#include <QQmlListProperty>
#include <QMetaProperty>
#include "RelayConnection.hpp"

#define PROP(type, field) type field; Q_PROPERTY(type field MEMBER field CONSTANT)

class WeechatLine {
public:
    static QHash<int, QByteArray> ROLE_NAMES;

    QDateTime timestamp;
    QByteArray prefix;
    QByteArray message;
    bool displayed;

    QString formatTimestamp() const {
        return timestamp.toString("hh:mm:ss");
    }

    enum Role {
        Timestamp,
        Prefix,
        Message,
        Displayed,
    };
};
QDebug operator<<(QDebug dbg, const WeechatLine& that);

class WeechatNick : public QObject {
Q_OBJECT
public:
    explicit WeechatNick(QObject* parent = nullptr) : QObject(parent) { }

    PROP(QByteArray, prefix);
    PROP(QByteArray, name);
};
QDebug operator<<(QDebug dbg, const WeechatNick& that);

class WeechatBuffer : public QAbstractListModel {
Q_OBJECT

public:
    explicit WeechatBuffer(QObject* parent = nullptr) : QAbstractListModel(parent) { }

    PROP(WPointer, id);
    PROP(int, number);
    PROP(QByteArray, fullName);
    PROP(QByteArray, shortName);
    PROP(QByteArray, title);
    PROP(QVariantHash, localVariables);

    QList<WeechatLine> lines;
    QList<WeechatNick*> nicks;

    virtual int rowCount(QModelIndex const&) const {
        return lines.length();
    }

    virtual QVariant data(const QModelIndex& index, int role) const {
        const WeechatLine& line = lines[index.row()];
        switch (role) {
            case WeechatLine::Timestamp:
                return line.formatTimestamp();
            case WeechatLine::Prefix:
                return line.prefix;
            case WeechatLine::Message:
                return line.message;
        }
        Q_ASSERT(0);
    }

    virtual QHash<int, QByteArray> roleNames() const {
        return WeechatLine::ROLE_NAMES;
    }

    void insertLine(WeechatLine& line, bool notify) {
        if (notify) {
            beginInsertRows(QModelIndex(), lines.length(), lines.length());
        }
        lines.push_back(line);
        if (notify) {
            endInsertRows();
        }
    }

private:
    Q_PROPERTY(QQmlListProperty<WeechatNick> nicks READ _getNicks CONSTANT);

    QQmlListProperty<WeechatNick> _getNicks() { return QQmlListProperty<WeechatNick>(this, nicks); }
};
QDebug operator<<(QDebug dbg, const WeechatBuffer& that);

class WeechatState : public QObject {
Q_OBJECT
    void process(QByteArray frameId, QVariant response);

Q_SIGNALS:
    void connectionEstablished();

public:
    explicit WeechatState(QObject* parent = nullptr) : QObject(parent) {
        connect(&relay, &RelayConnection::relayConnected, [=]{
            relay.writeString("init password=ffc54fe75cb3db7499544dfce520fd65589045756dd94d3bdb6bb38443ed7e61,compression=off\n");
            relay.writeString("(listbuffers) hdata buffer:gui_buffers(*) number,full_name,short_name,type,nicklist,title,local_variables\n");
            relay.writeString("(listlines) hdata buffer:gui_buffers(*)/own_lines/last_line(-120)/data date,displayed,prefix,message\n");
            relay.writeString("(nicklist) nicklist\n");
            relay.writeString("sync\n");
            relay.flush();
        });
        connect(&relay, &RelayConnection::relayMessageReceived, [=](QByteArray frameId, QVariant response){
            process(frameId, response);
        });
    }

    RelayConnection relay;
    QHash<WPointer, WeechatBuffer*> buffers;
    QList<WeechatBuffer*> buffersInOrder;

    Q_PROPERTY(QQmlListProperty<WeechatBuffer> buffers READ _getBuffersInOrder CONSTANT);

    QQmlListProperty<WeechatBuffer> _getBuffersInOrder() { return QQmlListProperty<WeechatBuffer>(this, buffersInOrder); }
};
