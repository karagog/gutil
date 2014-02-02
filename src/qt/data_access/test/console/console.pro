#-------------------------------------------------
#
# Project created by QtCreator 2010-10-19T21:04:26
#
#-------------------------------------------------

QT       += core

QT       += gui

TARGET = console
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../../../../include
LIBS += -L../../../../../lib -lGUtil -lGUtilQt

#DEFINES += GUTIL_NO_GUI_FUNCTIONALITY

SOURCES += main.cpp

HEADERS +=
