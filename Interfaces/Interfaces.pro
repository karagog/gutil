DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..

HEADERS += \
    iqxmlserializable.h \
    iqstringable.h \
    icollection.h
SOURCES += \
    iqxmlserializable.cpp \
    iqstringable.cpp
