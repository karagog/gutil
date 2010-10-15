#-------------------------------------------------
#
# Project created by QtCreator 2010-10-14T20:33:24
#
#-------------------------------------------------

QT       += network testlib

TARGET = tst_transportstest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_transportstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../..
LIBS += -L../../.. -lGUtil
