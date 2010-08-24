DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    widgethelpers.h \
    gthread.h

SOURCES += \
    widgethelpers.cpp \
    gthread.cpp

INCLUDEPATH += ../../../util
