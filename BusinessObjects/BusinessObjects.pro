# -------------------------------------------------
# Project created by QtCreator 2010-04-17T23:08:47
# -------------------------------------------------
QT += sql network
DESTDIR = ../lib
TEMPLATE = lib
CONFIG += staticlib
HEADERS += \
    BinaryDataStore.h \
    LocalClientServer.h \
    ConfigFile.h \
    abstractvaluebuffer.h \
    io_queue_worker.h

SOURCES += \
    BinaryDataStore.cpp \
    LocalClientServer.cpp \
    ConfigFile.cpp \
    abstractvaluebuffer.cpp \
    io_queue_worker.cpp

INCLUDEPATH += ..
