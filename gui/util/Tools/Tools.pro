DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    widgethelpers.h \
    cancellablethread.h

SOURCES += \
    widgethelpers.cpp \
    cancellablethread.cpp

INCLUDEPATH += ../../../util
