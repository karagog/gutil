#-------------------------------------------------
#
# Project created by QtCreator 2010-10-31T22:19:28
#
#-------------------------------------------------

QT       += gui network testlib

TARGET = tst_gvarianttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#DEFINES += GUTIL_NO_GUI_FUNCTIONALITY


SOURCES += tst_gvarianttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../..
LIBS += -L../../../lib \
        -lGUtil \
        -lGUtilCustom \
        -lGUtilInterfaces

DEFINES += DEBUG_LOGGING
