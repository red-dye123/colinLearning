QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#加载依赖库
LIBS += -lWs2_32

SOURCES += \
    chatdialog.cpp \
    friendlist.cpp \
    groupchatdialog.cpp \
    groupitem.cpp \
    main.cpp \
    imdialog.cpp \
    mediator/TcpClientMediator.cpp \
    mediator/mediator.cpp \
    net/TcpClient.cpp \
    qkernel.cpp \
    useritem.cpp

HEADERS += \
    chatdialog.h \
    friendlist.h \
    groupchatdialog.h \
    groupitem.h \
    imdialog.h \
    mediator/TcpClientMediator.h \
    mediator/mediator.h \
    net/TcpClient.h \
    net/def.h \
    net/net.h \
    qkernel.h \
    useritem.h

FORMS += \
    chatdialog.ui \
    friendlist.ui \
    groupchatdialog.ui \
    groupitem.ui \
    imdialog.ui \
    useritem.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
