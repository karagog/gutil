# -------------------------------------------------
# Project created by QtCreator 2010-03-27T16:39:45
# -------------------------------------------------

QT += network

TARGET = GUtilUtils

LIBS += -L../lib -lGUtilCore -lGUtilThirdParty

DESTDIR = ../lib
TEMPLATE = lib
#CONFIG += staticlib
SOURCES += \
    pubsubsystem.cpp \
    usermachinelocks.cpp \
    widgethelpers.cpp \
    qstringhelpers.cpp \
    universalmutex.cpp \
    datatransferutils.cpp

HEADERS += \
    pubsubsystem.h \
    usermachinelocks.h \
    widgethelpers.h \
    qstringhelpers.h \
    universalmutex.h \
    datatransferutils.h

INCLUDEPATH += ..
