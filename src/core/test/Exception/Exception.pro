#-------------------------------------------------
#
# Project created by QtCreator 2011-10-21T19:38:15
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_exceptiontest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += --std=c++11

TOP_DIR = ../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib -lGUtil

unix: LIBS += -Wl,-rpath,../../lib


SOURCES += tst_exceptiontest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
