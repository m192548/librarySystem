QT       += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/widget/addbookwidget.cpp \
    src/widget/adminwidget.cpp \
    src/widget/bookmanagementwidget.cpp \
    src/widget/borrowmanagementwidget.cpp \
    src/widget/loginwidget.cpp \
    src/main.cpp \
    src/widget/mainwindow.cpp \
    src/chart/mychart.cpp \
    src/net/net.cpp \
    src/widget/personalinformationwidget.cpp \
    src/widget/regeistwidget.cpp \
    src/widget/statisticalanalysiswidget.cpp \
    src/widget/userwidget.cpp

HEADERS += \
    src/widget/addbookwidget.h \
    src/widget/adminwidget.h \
    src/widget/bookmanagementwidget.h \
    src/widget/borrowmanagementwidget.h \
    src/widget/loginwidget.h \
    src/widget/mainwindow.h \
    src/chart/mychart.h \
    src/net/net.h \
    src/packet/packet.h \
    src/widget/personalinformationwidget.h \
    src/widget/regeistwidget.h \
    src/widget/statisticalanalysiswidget.h \
    src/widget/userwidget.h

FORMS += \
    src/widget/addbookwidget.ui \
    src/widget/adminwidget.ui \
    src/widget/bookmanagementwidget.ui \
    src/widget/borrowmanagementwidget.ui \
    src/widget/loginwidget.ui \
    src/widget/mainwindow.ui \
    src/widget/personalinformationwidget.ui \
    src/widget/regeistwidget.ui \
    src/widget/statisticalanalysiswidget.ui \
    src/widget/userwidget.ui

INCLUDEPATH += src/widget
INCLUDEPATH += src/net
INCLUDEPATH +=src/packet
INCLUDEPATH += src/chart
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/resource.qrc
