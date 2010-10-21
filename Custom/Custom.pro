DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    myflattreemodel.h \
    myxmlstreamreader.h \
    gthread.h \
    datacontainer.h \
    gsemaphore.h

SOURCES += \
    myflattreemodel.cpp \
    myxmlstreamreader.cpp \
    gthread.cpp \
    datacontainer.cpp \
    gsemaphore.cpp

INCLUDEPATH += ..
