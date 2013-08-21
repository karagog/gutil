#-------------------------------------------------
#
# Project created by QtCreator 2012-04-21T09:25:37
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_rngtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../../.. ../../../../include
LIBS += -L../../../../lib \
    -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_rngtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
