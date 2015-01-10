#-------------------------------------------------
#
# Project created by QtCreator 2011-09-23T16:59:56
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_vectortest
CONFIG   += console
CONFIG   -= app_bundle

QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app

TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib -lGUtil

unix: LIBS += -Wl,-rpath,$$TOP_DIR/lib


SOURCES += tst_vectortest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
