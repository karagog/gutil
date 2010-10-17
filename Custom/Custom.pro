DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    myflattreemodel.h \
    myxmlstreamreader.h \
    gthread.h \
    datacontainer.h

SOURCES += \
    myflattreemodel.cpp \
    myxmlstreamreader.cpp \
    gthread.cpp \
    datacontainer.cpp

INCLUDEPATH += ..
