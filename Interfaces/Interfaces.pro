DESTDIR = ../lib
QT -= gui

TARGET = GUtilInterfaces

TEMPLATE = lib
#CONFIG += staticlib

INCLUDEPATH += ..

HEADERS += \
    iqxmlserializable.h \
    iqstringable.h
SOURCES += \
    iqxmlserializable.cpp \
    iqstringable.cpp
