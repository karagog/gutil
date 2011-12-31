#-------------------------------------------------
#
# Project created by QtCreator 2010-10-12T21:23:52
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_myxmlstreamreadertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    xmlhelpers_test.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS +=

INCLUDEPATH += ../../..
LIBS += -L../../../ -lGUtil
