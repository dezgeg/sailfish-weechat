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
public:
    explicit WeechatState(QObject* parent = nullptr) : QObject(parent) { }

    RelayConnection relay;
    QHash<WPointer, WeechatBuffer*> buffers;
    QList<WeechatBuffer*> buffersInOrder;

    Q_PROPERTY(QQmlListProperty<WeechatBuffer> buffers READ _getBuffersInOrder CONSTANT);

    QQmlListProperty<WeechatBuffer> _getBuffersInOrder() { return QQmlListProperty<WeechatBuffer>(this, buffersInOrder); }

    void process();
};
