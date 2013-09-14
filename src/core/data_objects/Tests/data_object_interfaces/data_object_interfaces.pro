#-------------------------------------------------
#
# Project created by QtCreator 2013-09-14T18:00:36
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_data_object_interfacestest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib -lGUtil

unix: LIBS += -Wl,-rpath,$$TOP_DIR/lib

SOURCES += tst_data_object_interfacestest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
