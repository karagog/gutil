#-------------------------------------------------
#
# Project created by QtCreator 2011-12-23T10:17:28
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_encryptionutilstest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../../../include ../../..
LIBS += -L../../../../lib \
    -lGUtil


unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += \
    tst_cryptotest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
