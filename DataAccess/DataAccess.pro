DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    gfileiodevice.h \
    localsockettransport.h \
    giodevice.h \
    gconsoleiodevice.h \
    gqiodevice.h

SOURCES += \
    gfileiodevice.cpp \
    localsockettransport.cpp \
    giodevice.cpp \
    gconsoleiodevice.cpp \
    gqiodevice.cpp

INCLUDEPATH += ..
QT += sql network
