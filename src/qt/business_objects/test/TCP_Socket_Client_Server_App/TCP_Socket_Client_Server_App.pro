#-------------------------------------------------
#
# Project created by QtCreator 2010-12-22T15:38:02
#
#-------------------------------------------------

QT       += core gui network

TARGET = TCP_Client_Server_App
TEMPLATE = app

#DEFINES += GUTIL_NO_NETWORK_FUNCTIONALITY

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../..
LIBS += -L../../../lib \
    -lGUtilBusinessObjects
