#-------------------------------------------------
#
# Project created by QtCreator 2010-12-19T13:00:46
#
#-------------------------------------------------

QT       += core gui network

TARGET = tcp_socket_transport
TEMPLATE = app

DEFINES += NETWORK_FUNCTIONALITY

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../..
LIBS += -L../../../lib \
    -lGUtilThirdPartyQtLockedFile \
    -lGUtil \
    -lGUtilUtils \
    -lGUtilDataAccess

unix: LIBS += -Wl,-rpath,.
