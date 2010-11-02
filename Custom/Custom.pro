DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    myflattreemodel.h \
    gthread.h \
    gsemaphore.h \
    gvariant.h

SOURCES += \
    myflattreemodel.cpp \
    gthread.cpp \
    gsemaphore.cpp \
    gvariant.cpp

INCLUDEPATH += ..
