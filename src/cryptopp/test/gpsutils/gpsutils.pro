#-------------------------------------------------
#
# Project created by QtCreator 2014-09-07T10:28:42
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TOP_DIR = ../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib -L$$TOP_DIR/gutil/lib \
    -lGUtil \
    -lGUtilCryptoPP

TARGET = tst_gpsutilstest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += DEBUG


SOURCES += tst_gpsutilstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    payload1.txt
