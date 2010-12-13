DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    collection.h \
    gvariantcollection.h \
    explicitlysharedobject.h \
    updatablegvariantcollection.h

SOURCES += \
    updatablegvariantcollection.cpp

INCLUDEPATH += .. ../..

include(DataSet.pri)
