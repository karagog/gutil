#-------------------------------------------------
#
# Project created by QtCreator 2010-10-19T20:44:12
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = process_transport
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

INCLUDEPATH += ../../..
LIBS += -L../../../lib -lGUtil
