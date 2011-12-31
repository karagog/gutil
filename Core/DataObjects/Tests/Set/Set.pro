#-------------------------------------------------
#
# Project created by QtCreator 2011-09-13T19:38:09
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_settest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_settest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
