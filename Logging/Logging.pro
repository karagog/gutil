QT += sql

DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..

HEADERS += \
    igloballogger.h \
    globallogger.h \
    filelogger.h \
    consolelogger.h \
    abstractlogger.h \
    abstractdatabaselogger.h

SOURCES += \
    igloballogger.cpp \
    globallogger.cpp \
    filelogger.cpp \
    consolelogger.cpp \
    abstractlogger.cpp \
    abstractdatabaselogger.cpp
