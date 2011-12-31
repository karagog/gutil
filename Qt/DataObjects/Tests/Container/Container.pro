#-------------------------------------------------
#
# Project created by QtCreator 2010-11-14T10:48:42
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_containertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_containertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../..
LIBS += -L../../../lib -lGUtil

DEFINES += DEBUG_LOGGING
