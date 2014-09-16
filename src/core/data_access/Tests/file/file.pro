#-------------------------------------------------
#
# Project created by QtCreator 2011-12-04T08:41:10
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_filetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

TOP_DIR = ../../../../..

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_filetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
