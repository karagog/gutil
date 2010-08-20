DESTDIR = ../lib
TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    ixmlserializable.h \
    ITransportMechanism.h

SOURCES += \
    ITransportMechanism.cpp

INCLUDEPATH += .. ../../util
