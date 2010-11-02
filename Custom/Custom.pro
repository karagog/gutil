DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    myflattreemodel.h \
    myxmlstreamreader.h \
    gthread.h \
    datatable.h \
    gsemaphore.h \
    gvariant.h

SOURCES += \
    myflattreemodel.cpp \
    myxmlstreamreader.cpp \
    gthread.cpp \
    datatable.cpp \
    gsemaphore.cpp \
    gvariant.cpp

INCLUDEPATH += ..
