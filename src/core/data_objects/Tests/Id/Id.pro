#-------------------------------------------------
#
# Project created by QtCreator 2012-03-15T20:03:16
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_idtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib

SOURCES += tst_idtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
