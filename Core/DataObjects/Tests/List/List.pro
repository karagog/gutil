#-------------------------------------------------
#
# Project created by QtCreator 2011-10-01T17:38:25
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_listtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_listtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
