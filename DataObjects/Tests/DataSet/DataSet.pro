#-------------------------------------------------
#
# Project created by QtCreator 2010-11-04T23:04:32
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_datasettest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_datasettest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../..
LIBS += -L../../../lib -lGUtil

DEFINES += DEBUG_LOGGING

HEADERS += \
    persondatarow.h \
    peopletable.h \
    dogrow.h \
    dogtable.h \
    dogownerdataset.h
