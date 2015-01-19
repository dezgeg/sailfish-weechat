#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQuick/QQuickView>

#include "RelayConnection.hpp"
#include "WeechatState.hpp"
#include "FontMeasurer.hpp"

//#define SAILFISH
#ifdef SAILFISH
#include <sailfishapp.h>
#endif

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    qmlRegisterType<WeechatNick>("SailfishWeechat", 1, 0, "WeechatNick");
    qmlRegisterType<WeechatBuffer>("SailfishWeechat", 1, 0, "WeechatBuffer");
    qmlRegisterType<WeechatState>("SailfishWeechat", 1, 0, "WeechatState");

    qmlRegisterType<FontMeasurer>("SailfishWeechat", 1, 0, "FontMeasurer");

    WeechatState weechat;
    weechat.relay.connectToHost("localhost", 9001);

    QQuickView view;
    QObject::connect(&weechat, &WeechatState::connectionEstablished, [&] {
        view.rootContext()->setContextProperty("weechat", &weechat);
        view.setResizeMode(QQuickView::SizeRootObjectToView);
#ifdef SAILFISH
        view.setSource(SailfishApp::pathTo("qml/desktop/SailfishApp.qml"));
#else
        view.engine()->addImportPath("/home/tmtynkky/sailfish-weechat/qml/");
        view.engine()->addImportPath("/home/tmtynkky/sailfish-weechat/qml/desktop/");
        view.setSource(QUrl::fromLocalFile("/home/tmtynkky/sailfish-weechat/qml/desktop/DesktopApp.qml"));
#endif
        view.show();
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
    });

    return app.exec();
}
