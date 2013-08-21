#-------------------------------------------------
#
# Project created by QtCreator 2011-03-16T20:05:12
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_rangetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../../..
LIBS += -L../../../../lib \
    -lGUtil


SOURCES += \
    range_test.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
