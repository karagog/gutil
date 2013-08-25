#-------------------------------------------------
#
# Project created by QtCreator 2013-08-25T08:25:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = application
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib -lGUtil -lGUtilQt


SOURCES += main.cpp
