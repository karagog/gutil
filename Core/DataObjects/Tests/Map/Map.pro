#-------------------------------------------------
#
# Project created by QtCreator 2011-09-05T16:01:25
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_maptest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_maptest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
