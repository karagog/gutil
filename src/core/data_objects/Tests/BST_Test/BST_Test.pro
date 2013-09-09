#-------------------------------------------------
#
# Project created by QtCreator 2011-08-27T20:25:06
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_bst_testtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += \
    -L$$TOP_DIR/lib \
    -lGUtil

unix: LIBS += -Wl,-rpath,$$TOP_DIR/lib


SOURCES += tst_bst_testtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
