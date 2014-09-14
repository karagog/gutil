#-------------------------------------------------
#
# Project created by QtCreator 2014-09-14T10:17:02
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TOP_DIR = ../../../..

QMAKE_CXXFLAGS += -std=c++0x
INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib \
    -lGUtil

TARGET = tst_trycatchfinallytest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_trycatchfinallytest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
