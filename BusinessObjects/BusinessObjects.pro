# -------------------------------------------------
# Project created by QtCreator 2010-04-17T23:08:47
# -------------------------------------------------
QT += sql
DESTDIR = ../lib
TEMPLATE = lib
CONFIG += staticlib
HEADERS += \
    UserMachineLock.h \
    BinaryDataStore.h \
    LocalClientServer.h \
    ConfigFile.h \
    abstractvaluebuffer.h \
    filelogger.h \
    consolelogger.h \
    abstractlogger.h \
    abstractdatabaselogger.h \
    io_queue_worker.h \
    globallogger.h

SOURCES += \
    UserMachineLock.cpp \
    BinaryDataStore.cpp \
    LocalClientServer.cpp \
    ConfigFile.cpp \
    abstractvaluebuffer.cpp \
    filelogger.cpp \
    consolelogger.cpp \
    abstractlogger.cpp \
    abstractdatabaselogger.cpp \
    io_queue_worker.cpp \
    globallogger.cpp

INCLUDEPATH += .. ../include
