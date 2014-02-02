#-------------------------------------------------
#
# Project created by QtCreator 2010-10-21T12:14:31
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ConsoleRepeater
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../../../include
LIBS += -L../../../../lib \
    -lGUtil \
    -lGUtilQt

SOURCES += main.cpp \
    console_repeater.cpp

HEADERS += \
    console_repeater.h
