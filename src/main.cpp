#include <QApplication>
#include "RelayConnection.hpp"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    RelayConnection relay;
    relay.connect("localhost", 9001);

    relay.writeString("init password=ffc54fe75cb3db7499544dfce520fd65589045756dd94d3bdb6bb38443ed7e61,compression=off\n");
    relay.writeString("(listbuffers) hdata buffer:gui_buffers(*) number,full_name,short_name,type,nicklist,title,local_variables\n");
    relay.writeString("(listlines) hdata buffer:gui_buffers(*)/own_lines/last_line(-50)/data date,displayed,prefix,message\n");
    relay.writeString("(nicklist) nicklist\n");

//    relay.writeString("info version\n");
//    relay.writeString("ping foo\n");
//    relay.writeString("hdata buffer:gui_buffers(*)\n");
//    relay.writeString("nicklist\n");
//    relay.writeString("test\ntest\n");
    relay.flush();

    for (int i = 0; i < 3; i++) {
        QVariant reply = relay.readReply();
        if (reply.type() == QVariant::List) {
            for (QVariant& v : reply.toList()) {
                qDebug() << "    " << v;
            }
        } else {
            qDebug() << reply;
        }
    }
}
