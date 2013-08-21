#-------------------------------------------------
#
# Project created by QtCreator 2011-12-04T11:43:26
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_consoletest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_consoletest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
