#-------------------------------------------------
#
# Project created by QtCreator 2011-12-11T18:45:34
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_datatabletest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_datatabletest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
