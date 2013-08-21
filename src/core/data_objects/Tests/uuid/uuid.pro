#-------------------------------------------------
#
# Project created by QtCreator 2012-03-13T16:46:18
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_uuidtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#DEFINES += GUTIL_NO_CRYPTOPP

INCLUDEPATH += ../../../.. ../../../../include
LIBS += -L../../../../lib \
    -lGUtil \
    -lcryptopp

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_uuidtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
