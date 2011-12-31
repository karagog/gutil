#-------------------------------------------------
#
# Project created by QtCreator 2010-10-19T14:26:48
#
#-------------------------------------------------

QT       += core gui network

TARGET = UserMachineLock
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../..
LIBS += -L../../../lib -lGUtil
