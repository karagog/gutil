#-------------------------------------------------
#
# Project created by QtCreator 2012-05-02T16:50:01
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_cardstest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../.. ../../../include
LIBS += -L../../../lib \
    -lGUtilGames \
    -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib

SOURCES += tst_cardstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
