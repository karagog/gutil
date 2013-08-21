#-------------------------------------------------
#
# Project created by QtCreator 2011-10-21T19:38:15
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_exceptiontest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../..
LIBS += -L../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../lib


SOURCES += tst_exceptiontest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
