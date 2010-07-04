DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    widgethelpers.h \
    cancellablethread.h \
    filesystemhelpers.h

SOURCES += \
    widgethelpers.cpp \
    cancellablethread.cpp \
    filesystemhelpers.cpp

INCLUDEPATH += ../../../util
