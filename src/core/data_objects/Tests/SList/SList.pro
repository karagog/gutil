#-------------------------------------------------
#
# Project created by QtCreator 2011-09-03T17:34:27
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_slisttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += \
    tst_slisttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"




