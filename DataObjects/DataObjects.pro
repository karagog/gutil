DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    collection.h \
    gvariantcollection.h \
    explicitlysharedobject.h \
    updatablegvariantcollection.h \
    DataSet/sharedtabledata.h \
    DataSet/sharedsetdata.h \
    DataSet/sharedrowdata.h \
    DataSet/datatablecollection.h \
    DataSet/datatable.h \
    DataSet/dataset.h \
    DataSet/datarowcollection.h \
    DataSet/datarow.h \
    DataSet/datacolumncollection.h \
    DataSet/datacolumn.h

SOURCES += \
    updatablegvariantcollection.cpp \
    DataSet/sharedsetdata.cpp \
    DataSet/sharedrowdata.cpp \
    DataSet/datatablecollection.cpp \
    DataSet/dataset.cpp \
    DataSet/datarow.cpp \
    DataSet/datacolumncollection.cpp \
    DataSet/datacolumn.cpp \
    DataSet/datatable.cpp \
    DataSet/datarowcollection.cpp \
    DataSet/sharedtabledata.cpp

INCLUDEPATH += .. ../..
