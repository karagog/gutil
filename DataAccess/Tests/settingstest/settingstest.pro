#-------------------------------------------------
#
# Project created by QtCreator 2010-10-12T21:43:43
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_settingstesttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    settingstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS +=

INCLUDEPATH += ../../..
LIBS += -L../../../ -lGUtil
