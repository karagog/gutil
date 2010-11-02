DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    myflattreemodel.h \
    myxmlstreamreader.h \
    gthread.h \
    gsemaphore.h \
    gvariant.h

SOURCES += \
    myflattreemodel.cpp \
    myxmlstreamreader.cpp \
    gthread.cpp \
    gsemaphore.cpp \
    gvariant.cpp

INCLUDEPATH += ..
