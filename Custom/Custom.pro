DESTDIR = ../lib

TARGET = GUtilCustom

LIBS += -L../lib -lGUtilCore -lGUtilInterfaces -lGUtilUtils

TEMPLATE = lib
#CONFIG += staticlib

HEADERS += \
    myflattreemodel.h \
    gthread.h \
    gsemaphore.h \
    gsharedlock.h \
    gvariant.h \
    gshareddata.h \
    updatablegvariant.h \
    gpairlist.h

SOURCES += \
    myflattreemodel.cpp \
    gthread.cpp \
    gsemaphore.cpp \
    gsharedlock.cpp \
    gvariant.cpp \
    updatablegvariant.cpp

INCLUDEPATH += ..
