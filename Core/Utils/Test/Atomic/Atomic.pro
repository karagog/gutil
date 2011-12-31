#-------------------------------------------------
#
# Project created by QtCreator 2011-10-25T18:25:33
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_atomictest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_atomictest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
