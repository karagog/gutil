#-------------------------------------------------
#
# Project created by QtCreator 2011-09-23T16:59:56
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_vectortest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../../include ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_vectortest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
