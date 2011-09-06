#-------------------------------------------------
#
# Project created by QtCreator 2011-09-06T18:54:54
#
#-------------------------------------------------

QT       += core gui

TARGET = EnterString
TEMPLATE = app

INCLUDEPATH += ../.. ../../..
LIBS += -L../../../lib -lGUtilUi

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
