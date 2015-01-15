#include "FontMeasurer.hpp"
#include <QDebug>

int FontMeasurer::findMaxWidth(QFont font, QList<QString> strings) {
    QFontMetrics metrics(font);
    int widest = 0;
    for (const QString& str : strings) {
        widest = std::max(metrics.width(str), widest);
    }
    return widest;
}

int FontMeasurer::getFontHeight() {
    QFont font;
    font.setFamily(font.defaultFamily());

    QFontMetrics metrics(font);
    return metrics.height();
}
