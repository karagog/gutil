#-------------------------------------------------
#
# Project created by QtCreator 2011-10-01T09:01:09
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_flagstest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_flagstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
