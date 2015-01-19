TARGET = sailfish-weechat
TEMPLATE = app
CONFIG += sailfishapp c++11

SOURCES += \
    src/FontMeasurer.cpp \
    src/main.cpp \
    src/TerminalColors.cpp \
    src/WeechatState.cpp \
    src/RelayConnection.cpp \

HEADERS += \
    src/FontMeasurer.hpp \
    src/WeechatState.hpp \
    src/RelayConnection.hpp \
    src/TerminalColors.hpp \

OTHER_FILES += qml/sailfish-weechat.qml \
    qml/desktop/*.qml \
    rpm/sailfish-weechat.changes.in \
    rpm/sailfish-weechat.spec \
    rpm/sailfish-weechat.yaml \
    sailfish-weechat.desktop \
    qml/desktop/SailfishApp.qml
