#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQuick/QQuickView>

#include "RelayConnection.hpp"
#include "WeechatState.hpp"

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    qmlRegisterType<WeechatLine>("SailfishWeechat", 1, 0, "WeechatLine");
    qmlRegisterType<WeechatNick>("SailfishWeechat", 1, 0, "WeechatNick");
    qmlRegisterType<WeechatBuffer>("SailfishWeechat", 1, 0, "WeechatBuffer");
    qmlRegisterType<WeechatState>("SailfishWeechat", 1, 0, "WeechatState");

    WeechatState weechat;
    weechat.relay.connect("localhost", 9001);

    weechat.relay.writeString("init password=ffc54fe75cb3db7499544dfce520fd65589045756dd94d3bdb6bb38443ed7e61,compression=off\n");
    weechat.relay.writeString("(listbuffers) hdata buffer:gui_buffers(*) number,full_name,short_name,type,nicklist,title,local_variables\n");
    weechat.relay.writeString("(listlines) hdata buffer:gui_buffers(*)/own_lines/last_line(-60)/data date,displayed,prefix,message\n");
    weechat.relay.writeString("(nicklist) nicklist\n");

//    relay.writeString("info version\n");
//    relay.writeString("ping foo\n");
//    relay.writeString("hdata buffer:gui_buffers(*)\n");
//    relay.writeString("nicklist\n");
//    relay.writeString("test\ntest\n");
    weechat.relay.flush();

    for (int i = 0; i < 3; i++) {
        weechat.process();
    }

#if 0
    qDebug() << "Buffers:";
    for (auto buf : weechat.buffers) {
        qDebug() << *buf;
        for (auto nick : buf->nicks) {
            qDebug() << "   " << *nick;
        }
        for (auto line : buf->lines) {
            qDebug() << "   " << *line;
        }
        qDebug() << "";
    }
#endif

    QQuickView view;
    view.rootContext()->setContextProperty("weechat", &weechat);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    // XXX
    view.setSource(QUrl::fromLocalFile("/home/tmtynkky/sailfish-weechat/qml/desktop/App.qml"));
    view.show();

    return app.exec();
}
