#pragma once
#include <QFont>
#include <QFontMetrics>

class FontMeasurer : public QObject {
    Q_OBJECT
public:
    explicit FontMeasurer(QObject* parent = nullptr) : QObject(parent) { }

    Q_INVOKABLE int findMaxWidth(QFont font, QList<QString> strings);
    Q_INVOKABLE int getFontHeight();
};

