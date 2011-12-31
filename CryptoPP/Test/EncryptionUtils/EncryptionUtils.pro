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

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../lib -lGUtil -lGUtilCryptoPP

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_encryptionutilstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
