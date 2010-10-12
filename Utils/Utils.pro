# -------------------------------------------------
# Project created by QtCreator 2010-03-27T16:39:45
# -------------------------------------------------
QT += sql

DESTDIR = ../lib
TEMPLATE = lib
CONFIG += staticlib
SOURCES += \
    pubsubsystem.cpp \
    Private/abstractlogger.cpp \
    Private/abstractstreamlogger.cpp \
    filelogger.cpp \
    consolelogger.cpp \
    abstractdatabaselogger.cpp
HEADERS += \
    pubsubsystem.h \
    Private/abstractlogger.h \
    Private/abstractstreamlogger.h \
    filelogger.h \
    consolelogger.h \
    abstractdatabaselogger.h

INCLUDEPATH += .. .
