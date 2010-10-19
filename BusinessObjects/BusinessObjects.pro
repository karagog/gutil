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
    io_queue_worker.h

SOURCES += \
    UserMachineLock.cpp \
    BinaryDataStore.cpp \
    LocalClientServer.cpp \
    ConfigFile.cpp \
    abstractvaluebuffer.cpp \
    io_queue_worker.cpp

INCLUDEPATH += .. ../include
