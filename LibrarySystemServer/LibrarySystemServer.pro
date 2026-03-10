QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/db/db.cpp \
    src/main.cpp \
    src/widget/mainwindow.cpp \
    src/net/net.cpp

HEADERS += \
    src/db/db.h \
    src/widget/mainwindow.h \
    src/net/net.h \
    src/packet/packet.h

FORMS += \
    src/widget/mainwindow.ui

INCLUDEPATH +=src/widget
INCLUDEPATH +=src/packet
INCLUDEPATH +=src/net
INCLUDEPATH +=src/db

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
