#-------------------------------------------------
#
# Project created by QtCreator 2011-08-27T20:25:06
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_bst_testtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore


SOURCES += tst_bst_testtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
