#-------------------------------------------------
#
# Project created by QtCreator 2011-10-15T07:20:17
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_queuetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_queuetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
