#-------------------------------------------------
#
# Project created by QtCreator 2010-10-10T22:31:37
#
#-------------------------------------------------

QT       += testlib

QT      -= gui

TARGET = tst_loggertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_loggertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

DEFINES += GUTIL_DEBUG

INCLUDEPATH += ../../../../include ../../..
LIBS += -L../../../../lib \
    -lGUtil

unix: LIBS += -Wl,-rpath,.