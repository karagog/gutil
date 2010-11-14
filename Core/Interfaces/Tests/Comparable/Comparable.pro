#-------------------------------------------------
#
# Project created by QtCreator 2010-11-14T18:14:53
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_comparabletest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_comparabletest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../../..
LIBS += -L../../../../lib -lGUtil

DEFINES += DEBUG_LOGGING
