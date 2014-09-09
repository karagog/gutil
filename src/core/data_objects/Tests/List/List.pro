#-------------------------------------------------
#
# Project created by QtCreator 2011-10-01T17:38:25
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_listtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_listtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
