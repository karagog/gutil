DESTDIR = ../lib

TARGET = GUtilDataAccess
LIBS += -L../lib \
    -lGUtilCore \
    -lGUtilInterfaces \
    -lGUtilUtils \
    -lGUtilCustom \
    -lGUtilDataObjects

TEMPLATE = lib
#CONFIG += staticlib

HEADERS += \
    gfileiodevice.h \
    giodevice.h \
    gconsoleiodevice.h \
    gqiodevice.h \
    gdatabaseiodevice.h \
    glocalsocketiodevice.h \
    gtcpsocketiodevice.h

SOURCES += \
    gfileiodevice.cpp \
    giodevice.cpp \
    gconsoleiodevice.cpp \
    gqiodevice.cpp \
    gdatabaseiodevice.cpp \
    glocalsocketiodevice.cpp \
    gtcpsocketiodevice.cpp

INCLUDEPATH += ..

# Uncomment these to enable database functionality
#QT += sql
#DEFINES += DATABASE_FUNCTIONALITY

#QT += network
#DEFINES += NETWORK_FUNCTIONALITY

QT -= gui
#DEFINES += GUI_FUNCTIONALITY
