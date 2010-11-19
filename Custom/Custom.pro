DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    myflattreemodel.h \
    gthread.h \
    gsemaphore.h \
    gshareddatapointer.h \
    gsharedlock.h \
    gvariant.h

SOURCES += \
    myflattreemodel.cpp \
    gthread.cpp \
    gsemaphore.cpp \
    gsharedlock.cpp \
    gvariant.cpp

INCLUDEPATH += ..
