#-------------------------------------------------
#
# Project created by QtCreator 2014-09-13T09:29:14
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TOP_DIR = ../../../..
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += $$TOP_DIR/include
TARGET = tst_foreachtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_foreachtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
