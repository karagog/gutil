#-------------------------------------------------
#
# Project created by QtCreator 2011-09-03T17:34:27
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_slisttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib -lGUtil

unix: LIBS += -Wl,-rpath,$$TOP_DIR/lib


SOURCES += \
    tst_slisttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"




