#-------------------------------------------------
#
# Project created by QtCreator 2010-10-12T21:43:43
#
#-------------------------------------------------

QT       += testlib

TARGET = usermachinelocktest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    usermachinelock_test.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../..
LIBS += -L../../../ -lGUtil
