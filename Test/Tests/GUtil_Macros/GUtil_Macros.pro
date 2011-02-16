#-------------------------------------------------
#
# Project created by QtCreator 2010-11-16T15:21:40
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_gutil_macrostest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_gutil_macrostest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


INCLUDEPATH += ../..
LIBS += -L../../lib -lGUtil

DEFINES += DEBUG_LOGGING
