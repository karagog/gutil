DESTDIR = ../lib

TARGET = GUtilDataObjects
LIBS += -L../lib -lGUtilCore -lGUtilInterfaces -lGUtilCustom -lGUtilUtils

TEMPLATE = lib
#CONFIG += staticlib

HEADERS += \
    collection.h \
    gvariantcollection.h \
    explicitlysharedobject.h \
    updatablegvariantcollection.h \
    timeregion.h

SOURCES += \
    updatablegvariantcollection.cpp

INCLUDEPATH += .. ../..

include(DataSet/DataSet.pri)
