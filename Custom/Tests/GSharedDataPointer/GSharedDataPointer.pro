#-------------------------------------------------
#
# Project created by QtCreator 2010-11-07T17:56:11
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_gshareddatapointertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_gshareddatapointertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../..
LIBS += -L../../.. -lGUtil
