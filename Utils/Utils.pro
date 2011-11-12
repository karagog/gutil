# -------------------------------------------------
# Project created by QtCreator 2010-03-27T16:39:45
# -------------------------------------------------

TARGET = GUtilUtils

LIBS += -L../lib \
    -lGUtilCore \
    -lGUtilThirdPartyQt

DESTDIR = ../lib
TEMPLATE = lib
#CONFIG += staticlib

# Uncomment these if you need the database functions
QT += sql
DEFINES += DATABASE_FUNCTIONALITY

#QT -= gui
DEFINES += GUI_FUNCTIONALITY

QT += network
DEFINES += NETWORK_FUNCTIONALITY

SOURCES += \
    pubsubsystem.cpp \
    usermachinelocks.cpp \
    universalmutex.cpp \
    datatransferutils.cpp \
    databaseutils.cpp \
    stopwatchengine.cpp \
    globalmemorymanager.cpp \
    commandlinemenu.cpp

HEADERS += \
    pubsubsystem.h \
    usermachinelocks.h \
    universalmutex.h \
    datatransferutils.h \
    response.h \
    synchronizationobject.h \
    databaseutils.h \
    stopwatchengine.h \
    globalmemorymanager.h \
    commandlinemenu.h

INCLUDEPATH += ..


