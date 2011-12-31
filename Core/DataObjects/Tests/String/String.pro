#-------------------------------------------------
#
# Project created by QtCreator 2011-10-22T14:31:35
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_stringtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# Enable this define to have access to functions that will convert between Qt types and GUtil types
DEFINES += GUTIL_CORE_QT_ADAPTERS

INCLUDEPATH += ../.. ../../../.. ../../..
LIBS += -L../../../../lib -lGUtil

unix: LIBS += -Wl,-rpath,../../../../lib


SOURCES += tst_stringtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
