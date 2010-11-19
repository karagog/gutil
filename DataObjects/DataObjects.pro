DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    datatable.h \
    dataset.h \
    datarow.h \
    collection.h \
    gvariantcollection.h \
    shareddataobjects.h \
    datarowcollection.h \
    datatablecollection.h \
    datacolumncollection.h \
    datacolumn.h \
    datatablecollectionbase.h \
    datarowcollectionbase.h \
    datacollections.h

SOURCES += \
    datatable.cpp \
    dataset.cpp \
    datarow.cpp \
    shareddataobjects.cpp \
    datacolumncollection.cpp \
    datacolumn.cpp

INCLUDEPATH += ..
