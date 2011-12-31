#-------------------------------------------------
#
# Project created by QtCreator 2011-10-29T10:12:00
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_shareddatapointertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_shareddatapointertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
