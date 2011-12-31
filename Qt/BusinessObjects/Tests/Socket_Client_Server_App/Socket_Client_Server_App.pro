#-------------------------------------------------
#
# Project created by QtCreator 2010-12-22T15:38:02
#
#-------------------------------------------------

QT       += core gui network

TARGET = Socket_Client_Server_App
TEMPLATE = app

DEFINES += NETWORK_FUNCTIONALITY

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../..
LIBS += -L../../../lib \
    -lGUtilBusinessObjects
