#-------------------------------------------------
#
# Project created by QtCreator 2010-12-12T16:38:30
#
#-------------------------------------------------

QT       += core gui network

TARGET = Config_Test_App
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../..
LIBS += -L../../../lib -lGUtil
