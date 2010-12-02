DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    datatablebase.h \
    dataset.h \
    datarow.h \
    collection.h \
    gvariantcollection.h \
    sharedsetdata.h \
    datacolumncollection.h \
    datacolumn.h \
    datatablecollection.h \
    datarowcollectionbase.h \
    datacollections.h \
    datarowcollection.h \
    datatable.h \
    sharedtabledata.h \
    sharedrowdata.h \
    explicitlysharedobject.h \
    updatablegvariantcollection.h \
    updatablegvariant.h

SOURCES += \
    dataset.cpp \
    datarow.cpp \
    sharedsetdata.cpp \
    datacolumncollection.cpp \
    datacolumn.cpp \
    datatablecollection.cpp \
    sharedrowdata.cpp \
    updatablegvariantcollection.cpp

INCLUDEPATH += ..
