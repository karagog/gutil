#-------------------------------------------------
#
# Project created by QtCreator 2010-12-13T11:37:40
#
#-------------------------------------------------

QT       += core gui

TARGET = UniversalLock
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../..
LIBS += -L../../.. -lGUtil
