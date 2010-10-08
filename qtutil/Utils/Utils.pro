# -------------------------------------------------
# Project created by QtCreator 2010-03-27T16:39:45
# -------------------------------------------------
DESTDIR = ../lib
TEMPLATE = lib
CONFIG += staticlib
SOURCES += \
    pubsubsystem.cpp \
    streamlogger.cpp \
    Private/abstractlogger.cpp
HEADERS += \
    pubsubsystem.h \
    streamlogger.h \
    Private/abstractlogger.h

INCLUDEPATH += ../../util .
