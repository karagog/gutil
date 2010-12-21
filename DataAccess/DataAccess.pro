DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    gfileiodevice.h \
    giodevice.h \
    gconsoleiodevice.h \
    gqiodevice.h \
    gdatabaseiodevice.h \
    gsocketiodevice.h

SOURCES += \
    gfileiodevice.cpp \
    giodevice.cpp \
    gconsoleiodevice.cpp \
    gqiodevice.cpp \
    gdatabaseiodevice.cpp \
    gsocketiodevice.cpp

INCLUDEPATH += ..
QT += sql network
