#-------------------------------------------------
#
# Project created by QtCreator 2011-09-17T10:18:58
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_dlisttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_dlisttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
