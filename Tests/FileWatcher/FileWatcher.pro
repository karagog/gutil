#-------------------------------------------------
#
# Project created by QtCreator 2010-10-16T11:43:50
#
#-------------------------------------------------

QT       += core gui

TARGET = FileWatcher
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../..
LIBS += -L../../lib -lGUtil
