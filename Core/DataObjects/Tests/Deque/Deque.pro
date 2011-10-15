#-------------------------------------------------
#
# Project created by QtCreator 2011-10-15T07:20:38
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_dequetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_dequetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
