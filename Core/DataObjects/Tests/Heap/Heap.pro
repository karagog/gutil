#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T11:47:16
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_heaptest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_heaptest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
