#-------------------------------------------------
#
# Project created by QtCreator 2019-06-18T16:47:26
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Messaging
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
CONFIG += link_pkgconfig

packagesExist(glib-2.0) {
    DEFINES += HAS_GLIB
    PKGCONFIG += glib-2.0
}

@SOURCES = $$files(src/*.cpp)@
@HEADERS = $$files(src/*.h)@
@FORMS = $$files(src/forms/*.ui)@

SOURCES += src/crashwindow.cpp \
    src/client.cpp \
    src/config.cpp \
    src/connection.cpp \
    src/chatdialog.cpp \
    src/mainwindow.cpp \
    src/messagenode.cpp \
    src/network.cpp \
    src/peermanager.cpp \
    src/server.cpp \
    src/startupdebug.cpp \
    src/main.cpp\
    src/logger.cpp \
    src/theapp.cpp

HEADERS += src/crashwindow.h \
    src/client.h \
    src/config.h \
    src/connection.h \
    src/chatdialog.h \
    src/mainwindow.h \
    src/messagenode.h \
    src/network.h \
    src/peermanager.h \
    src/server.h \
    src/startupdebug.h \
    src/logger.h \
    src/theapp.h

FORMS += src/forms/crashwindow.ui \
    src/forms/chatdialog.ui \
    src/forms/mainwindow.ui \
    src/forms/messagenode.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
