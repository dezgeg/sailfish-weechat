#include "TerminalColors.hpp"
#include <QString>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QUrl>

#include <stdint.h>
#include <ctype.h>

// ANSI:
// black, red, green, yellow, blue, magenta, cyan, white
// 0x000000, 0x800000, 0x008000, 0x808000, 0x000080, 0x800080, 0x008080, 0xc0c0c0,
// 0x808080, 0xff0000, 0x00ff00, 0xffff00, 0x0000ff, 0xff00ff, 0x00ffff, 0xffffff,

const uint32_t terminalColors[] = {
        // WeeChat:
        // white, black, dgray, dred, lred, dgreen, lgreen, brown
        // yellow, dblue, lblue, dmagenta, lmagenta, dcyan, lcyan, gray, white
        0xc0c0c0, 0x000000, 0x808080, 0x800000, 0xff0000, 0x008000, 0x00ff80, 0x808000,
        0xffff00, 0x000080, 0x0000ff, 0x800080, 0xff00ff, 0x008080, 0x00ffff, 0xffffff,

        0x000000, 0x00005f, 0x000087, 0x0000af, 0x0000d7, 0x0000ff, 0x005f00, 0x005f5f,
        0x005f87, 0x005faf, 0x005fd7, 0x005fff, 0x008700, 0x00875f, 0x008787, 0x0087af,
        0x0087d7, 0x0087ff, 0x00af00, 0x00af5f, 0x00af87, 0x00afaf, 0x00afd7, 0x00afff,
        0x00d700, 0x00d75f, 0x00d787, 0x00d7af, 0x00d7d7, 0x00d7ff, 0x00ff00, 0x00ff5f,
        0x00ff87, 0x00ffaf, 0x00ffd7, 0x00ffff, 0x5f0000, 0x5f005f, 0x5f0087, 0x5f00af,
        0x5f00d7, 0x5f00ff, 0x5f5f00, 0x5f5f5f, 0x5f5f87, 0x5f5faf, 0x5f5fd7, 0x5f5fff,
        0x5f8700, 0x5f875f, 0x5f8787, 0x5f87af, 0x5f87d7, 0x5f87ff, 0x5faf00, 0x5faf5f,
        0x5faf87, 0x5fafaf, 0x5fafd7, 0x5fafff, 0x5fd700, 0x5fd75f, 0x5fd787, 0x5fd7af,
        0x5fd7d7, 0x5fd7ff, 0x5fff00, 0x5fff5f, 0x5fff87, 0x5fffaf, 0x5fffd7, 0x5fffff,
        0x870000, 0x87005f, 0x870087, 0x8700af, 0x8700d7, 0x8700ff, 0x875f00, 0x875f5f,
        0x875f87, 0x875faf, 0x875fd7, 0x875fff, 0x878700, 0x87875f, 0x878787, 0x8787af,
        0x8787d7, 0x8787ff, 0x87af00, 0x87af5f, 0x87af87, 0x87afaf, 0x87afd7, 0x87afff,
        0x87d700, 0x87d75f, 0x87d787, 0x87d7af, 0x87d7d7, 0x87d7ff, 0x87ff00, 0x87ff5f,
        0x87ff87, 0x87ffaf, 0x87ffd7, 0x87ffff, 0xaf0000, 0xaf005f, 0xaf0087, 0xaf00af,
        0xaf00d7, 0xaf00ff, 0xaf5f00, 0xaf5f5f, 0xaf5f87, 0xaf5faf, 0xaf5fd7, 0xaf5fff,
        0xaf8700, 0xaf875f, 0xaf8787, 0xaf87af, 0xaf87d7, 0xaf87ff, 0xafaf00, 0xafaf5f,
        0xafaf87, 0xafafaf, 0xafafd7, 0xafafff, 0xafd700, 0xafd75f, 0xafd787, 0xafd7af,
        0xafd7d7, 0xafd7ff, 0xafff00, 0xafff5f, 0xafff87, 0xafffaf, 0xafffd7, 0xafffff,
        0xd70000, 0xd7005f, 0xd70087, 0xd700af, 0xd700d7, 0xd700ff, 0xd75f00, 0xd75f5f,
        0xd75f87, 0xd75faf, 0xd75fd7, 0xd75fff, 0xd78700, 0xd7875f, 0xd78787, 0xd787af,
        0xd787d7, 0xd787ff, 0xd7af00, 0xd7af5f, 0xd7af87, 0xd7afaf, 0xd7afd7, 0xd7afff,
        0xd7d700, 0xd7d75f, 0xd7d787, 0xd7d7af, 0xd7d7d7, 0xd7d7ff, 0xd7ff00, 0xd7ff5f,
        0xd7ff87, 0xd7ffaf, 0xd7ffd7, 0xd7ffff, 0xff0000, 0xff005f, 0xff0087, 0xff00af,
        0xff00d7, 0xff00ff, 0xff5f00, 0xff5f5f, 0xff5f87, 0xff5faf, 0xff5fd7, 0xff5fff,
        0xff8700, 0xff875f, 0xff8787, 0xff87af, 0xff87d7, 0xff87ff, 0xffaf00, 0xffaf5f,
        0xffaf87, 0xffafaf, 0xffafd7, 0xffafff, 0xffd700, 0xffd75f, 0xffd787, 0xffd7af,
        0xffd7d7, 0xffd7ff, 0xffff00, 0xffff5f, 0xffff87, 0xffffaf, 0xffffd7, 0xffffff,
        0x080808, 0x121212, 0x1c1c1c, 0x262626, 0x303030, 0x3a3a3a, 0x444444, 0x4e4e4e,
        0x585858, 0x626262, 0x6c6c6c, 0x767676, 0x808080, 0x8a8a8a, 0x949494, 0x9e9e9e,
        0xa8a8a8, 0xb2b2b2, 0xbcbcbc, 0xc6c6c6, 0xd0d0d0, 0xdadada, 0xe4e4e4, 0xeeeeee,
};

static const uint32_t weechatInternalColors[] = {
        0x000066, // separator
        0x000000, // chat *
        0x999999, // chat_time
        0x000000, // chat_time_delimiters
        0xff6633, // chat_prefix_error *
        0x990099, // chat_prefix_network *
        0x000000, // chat_prefix_action *
        0x00cc00, // chat_prefix_join *
        0xcc0000, // chat_prefix_quit *
        0xcc00ff, // chat_prefix_more
        0x330099, // chat_prefix_suffix
        0x000000, // chat_buffer *
        0x000000, // chat_server
        0x000000, // chat_channel *
        0x000000, // chat_nick
        0xffffff, // chat_nick_self *
        0x000000, // chat_nick_other
        0x000000, // nick1
        0x000000, // nick2
        0x000000, // nick3
        0x000000, // nick4
        0x000000, // nick5
        0x000000, // nick6
        0x000000, // nick7
        0x000000, // nick8
        0x000000, // nick9
        0x000000, // nick10
        0x666666, // chat_host *
        0x9999ff, // chat_delimiters *
        0x3399cc, // chat_highlight
        0x000000, // chat_read_marker
        0x000000, // chat_text_found
        0x000000, // chat_value *
        0x000000, // chat_prefix_buffer
        0x000000, // chat_tags
        0x000000, // chat_inactive_window
        0x000000, // chat_inactive_buffer
        0x000000, // chat_prefix_buffer_inactive_buffer
        0x000000, // chat_nick_offline
        0x000000, // chat_nick_offline_highlight
        0x000000, // chat_nick_prefix
        0x000000, // chat_nick_suffix
        0x000000, // emphasis
        0x000000, // chat_day_change
};

static int parseOneColor(QByteArray const& str, int i, int* pNewColor, char* pNewAttr) {
    int numDigits = 2;
    if (str[i] == '@') {
        qDebug() << "ext color used" << str;
        numDigits = 5;
        i++;
    }
    *pNewAttr = '|';
    if (index("*!/_|", str[i])) {
        *pNewAttr = str[i];
        i++;
    }
    int color = 0;
    for (int j = 0; j < numDigits; j++) {
        color = color * 10 + str[i++] - '0';
    }
    *pNewColor = color;
    return i - 1;
}

QByteArray convertColorCodes(const QByteArray& str) {
    QByteArray ret;
    ret.append("<html><font color=\"#ffffff\">");

    for (int i = 0; i < str.size(); i++) {
        char c = str[i];
        if (c == 0x19) {
            i++;
            char escape = str[i];
            if (escape == 'F' || escape == '*') {
                i++;
                int newColor;
                char newAttr;
                i = parseOneColor(str, i, &newColor, &newAttr);
                ret += QString().sprintf("</font><font color=\"#%06x\">", terminalColors[newColor]);
//                qDebug() << "color code used: " << newColor << str;
            } else if (isdigit(escape)) {
                int index = 10 * (escape - '0') + str[++i] - '0';
                ret += QString().sprintf("</font><font color=\"#%06x\">", weechatInternalColors[index]);
//                qDebug() << "internal color used: " << index << str;
            } else {
                qDebug() << "Unknown byte after 0x19 escape: " << int(escape);
            }
        } else if (c >= 0x1a && c <= 0x1c) {
            qDebug() << "Unhandled color escape: " << int(str[i]);
        } else if (c == '<') {
            ret.append("&lt;");
        } else if (c == '>') {
            ret.append("&gt;");
        } else {
            ret.push_back(c);
        }
    }
    ret.append("</font></html>");
    return ret;
}

// XXX: not in this file
QByteArray convertUrls(QByteArray bytes) {
    QString str = bytes;
    // https://github.com/OlliV/thumbterm/commit/074e83f775c79ea13bab501717c9f43a95708b07 with slight modifications
    static QRegularExpression re("("
            "(" // brackets covering match for protocol (optional) and domain
            "(https?:(?:\\/\\/)?)" // match protocol, allow in format http:// or mailto:
            "(?:[\\-;:&=\\+\\$,\\w]+@)?" // allow something@ for email addresses
            "[A-Za-z0-9\\.\\-]+" // anything looking at all like a domain, non-unicode domains
            "|" // or instead of above
            "(?:www\\.)" // starting with www.
            "[A-Za-z0-9\\.\\-]+"   // anything looking at all like a domain
            ")"
            "(" // brackets covering match for path, query string and anchor
            "(?:\\/[\\+~%\\/\\.\\w\\-]*)" // allow optional /path
            "?\\?""?(?:[\\-\\+=&;%@\\.\\w]*)" // allow optional query string starting with ?
            "#?(?:[\\.\\!\\/\\\\\\w]*)" // allow optional anchor #anchor
            ")?" // make URL suffix optional
            ")"
    );

    return str.replace(re, "<a href=\"\\1\">\\1</a>").toUtf8();
}
