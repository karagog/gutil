#-------------------------------------------------
#
# Project created by QtCreator 2011-09-03T17:34:27
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_stacktest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_stacktest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
