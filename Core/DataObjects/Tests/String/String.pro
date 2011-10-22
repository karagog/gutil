#-------------------------------------------------
#
# Project created by QtCreator 2011-10-22T14:31:35
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_stringtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_stringtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
