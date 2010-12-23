# -------------------------------------------------
# Project created by QtCreator 2010-04-17T23:08:47
# -------------------------------------------------
DESTDIR = ../lib

TARGET = GUtilControls

TEMPLATE = lib
#CONFIG += staticlib
HEADERS += progressbarcontrol.h \
    filechooser.h
SOURCES += progressbarcontrol.cpp \
    filechooser.cpp

INCLUDEPATH += ..

# All the controls are built off the controls in Core
include(Core/Core.pri)
