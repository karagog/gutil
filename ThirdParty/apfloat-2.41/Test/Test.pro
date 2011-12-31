#-------------------------------------------------
#
# Project created by QtCreator 2011-12-23T14:39:59
#
#-------------------------------------------------

QT       -= core gui

TARGET = Test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -w

INCLUDEPATH += ..
LIBS += -L../../../lib -lapfloat

SOURCES += \
    aptest.cpp
