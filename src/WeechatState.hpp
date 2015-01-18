#pragma once

#include <algorithm>
#include <QDebug>
#include <QDateTime>
#include <QObject>
#include <QQmlListProperty>
#include "RelayConnection.hpp"

#define PROP(type, field) type field; Q_PROPERTY(type field MEMBER field CONSTANT)

class WeechatLine : public QObject {
Q_OBJECT
public:
    explicit WeechatLine(QObject* parent = nullptr) : QObject(parent) { }

    PROP(QDateTime, timestamp);
    PROP(QByteArray, prefix);
    PROP(QByteArray, message);
    PROP(bool, displayed);

    Q_INVOKABLE QString formatTimestamp() const {
        return timestamp.toString("hh:mm:ss");
    }
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

class WeechatBuffer : public QObject {
Q_OBJECT
public:
    explicit WeechatBuffer(QObject* parent = nullptr) : QObject(parent) { }

    PROP(WPointer, id);
    PROP(int, number);
    PROP(QByteArray, fullName);
    PROP(QByteArray, shortName);
    PROP(QByteArray, title);
    PROP(QVariantHash, localVariables);

    QList<WeechatLine*> lines;
    QList<WeechatNick*> nicks;

private:
    Q_PROPERTY(QQmlListProperty<WeechatLine> lines READ _getLines CONSTANT);
    Q_PROPERTY(QQmlListProperty<WeechatNick> nicks READ _getNicks CONSTANT);

    QQmlListProperty<WeechatLine> _getLines() { return QQmlListProperty<WeechatLine>(this, lines); }
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
            relay.writeString("(listlines) hdata buffer:gui_buffers(*)/own_lines/last_line(-60)/data date,displayed,prefix,message\n");
            relay.writeString("(nicklist) nicklist\n");
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
