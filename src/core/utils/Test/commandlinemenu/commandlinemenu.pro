#-------------------------------------------------
#
# Project created by QtCreator 2011-12-05T19:17:40
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_commandlinemenu
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib

SOURCES += tst_pubsubsystemtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
