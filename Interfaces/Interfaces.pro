DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..

HEADERS += \
    iqxmlserializable.h \
    iqstringable.h \
    iupdatable.h
SOURCES += \
    iqxmlserializable.cpp \
    iqstringable.cpp \
    iupdatable.cpp
