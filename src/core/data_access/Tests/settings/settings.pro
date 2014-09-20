#-------------------------------------------------
#
# Project created by QtCreator 2014-09-20T10:51:44
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_settingstest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
QMAKE_CXXFLAGS += --std=c++11
TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib \
    -lGUtil

SOURCES += tst_settingstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
