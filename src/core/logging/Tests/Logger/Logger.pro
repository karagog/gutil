#-------------------------------------------------
#
# Project created by QtCreator 2010-10-10T22:31:37
#
#-------------------------------------------------

QT       += testlib

QT      -= gui

TARGET = tst_loggertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += --std=c++11

SOURCES += tst_loggertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

DEFINES += GUTIL_DEBUG

TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib \
    -lGUtil

unix: LIBS += -Wl,-rpath,.
