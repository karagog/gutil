QT += sql

DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..

HEADERS += \
    globallogger.h \
    filelogger.h \
    consolelogger.h \
    abstractlogger.h \
    abstractdatabaselogger.h \
    messageboxlogger.h \
    grouplogger.h \
    debuglogger.h

SOURCES += \
    globallogger.cpp \
    filelogger.cpp \
    consolelogger.cpp \
    abstractlogger.cpp \
    abstractdatabaselogger.cpp \
    messageboxlogger.cpp \
    grouplogger.cpp \
    debuglogger.cpp

DEFINES += DEBUG_LOGGING
