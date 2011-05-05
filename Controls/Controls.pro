# -------------------------------------------------
# Project created by QtCreator 2010-04-17T23:08:47
# -------------------------------------------------
DESTDIR = ../lib

TARGET = GUtilControls

TEMPLATE = lib
#CONFIG += staticlib
HEADERS += \
    progressbarcontrol.h \
    filechooser.h \
    line_edit_with_button.h \
    faderwidget.h \
    expander.h \
    stopwatch.h \
    timerangepicker.h
SOURCES += \
    progressbarcontrol.cpp \
    filechooser.cpp \
    line_edit_with_button.cpp \
    faderwidget.cpp \
    expander.cpp \
    stopwatch.cpp \
    timerangepicker.cpp

INCLUDEPATH += ..
LIBS += \
    -L../lib \
    -lGUtilUtils \
    -lGUtilCustom
