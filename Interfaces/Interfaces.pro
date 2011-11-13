DESTDIR = ../lib
QT -= gui

TARGET = GUtilInterfaces

DEFINES += GUTIL_CORE_QT_ADAPTERS

TEMPLATE = lib
#CONFIG += staticlib

INCLUDEPATH += ..
LIBS += -L../lib \
        -lGUtilCore

HEADERS += \
    iqxmlserializable.h \
    iqstringable.h
SOURCES += \
    iqxmlserializable.cpp \
    iqstringable.cpp
