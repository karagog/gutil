#-------------------------------------------------
#
# Project created by QtCreator 2011-03-16T20:05:12
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_gdatetimetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../..
LIBS += -L../../../lib \
    -lGUtilCustom


SOURCES += tst_gdatetimetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
