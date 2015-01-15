#include "FontMeasurer.hpp"

int FontMeasurer::findMaxWidth(QList<QString> strings) {
    QFont font;
    font.setFamily(font.defaultFamily());
    QFontMetrics metrics(font);

    int widest = 0;
    for (const QString& str : strings) {
        widest = std::max(metrics.width(str), widest);
    }

    return widest;
}
