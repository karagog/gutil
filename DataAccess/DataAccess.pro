DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    gfileiodevice.h \
    localsockettransport.h \
    giodevice.h \
    gconsoleiodevice.h \
    gqiodevice.h \
    gdatabaseiodevice.h

SOURCES += \
    gfileiodevice.cpp \
    localsockettransport.cpp \
    giodevice.cpp \
    gconsoleiodevice.cpp \
    gqiodevice.cpp \
    gdatabaseiodevice.cpp

INCLUDEPATH += ..
QT += sql network
