DESTDIR = ../lib
QT -= gui

TARGET = GUtilDataObjects
LIBS += -L../lib -lGUtilCore -lGUtilInterfaces -lGUtilCustom -lGUtilUtils

DEFINES += GUTIL_CORE_QT_ADAPTERS

TEMPLATE = lib
#CONFIG += staticlib

HEADERS += \
    collection.h \
    gvariantcollection.h \
    explicitlysharedobject.h \
    updatablegvariantcollection.h \
    timeregion.h \
    version.h

SOURCES += \
    updatablegvariantcollection.cpp \
    version.cpp

INCLUDEPATH += .. ../..

include(DataSet/DataSet.pri)
