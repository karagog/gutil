#-------------------------------------------------
#
# Project created by QtCreator 2011-09-20T09:40:46
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_benchmark_liststest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtilCore

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_benchmark_liststest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
