#pragma once
#include <QFont>
#include <QFontMetrics>

class FontMeasurer : public QObject {
    Q_OBJECT
public:
    explicit FontMeasurer(QObject* parent = nullptr) : QObject(parent) { }

    Q_INVOKABLE int findMaxWidth(QList<QString> strings);
};

