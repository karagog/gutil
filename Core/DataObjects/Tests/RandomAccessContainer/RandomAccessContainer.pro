#-------------------------------------------------
#
# Project created by QtCreator 2011-10-15T07:20:57
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_randomaccesscontainertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_randomaccesscontainertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
