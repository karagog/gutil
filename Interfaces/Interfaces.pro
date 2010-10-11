DESTDIR = ../lib
TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    ixmlserializable.h \
    ITransportMechanism.h \
    ireadonlyobject.h

SOURCES += \
    ITransportMechanism.cpp \
    ireadonlyobject.cpp

INCLUDEPATH += .. ../../util
