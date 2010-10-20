#-------------------------------------------------
#
# Project created by QtCreator 2010-10-19T21:04:26
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = console
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../..
LIBS += -L../../../lib -lGUtil

SOURCES += main.cpp

HEADERS += \
    receiver_T.h
