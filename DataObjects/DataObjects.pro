DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    datatable.h \
    dataset.h \
    qvarianthelpers.h \
    datarow.h \
    collection.h \
    qvariantcollection.h \
    shareddataobjects.h \
    datarowcollection.h \
    datatablecollection.h \
    columncollection.h

SOURCES += \
    datatable.cpp \
    dataset.cpp \
    qvarianthelpers.cpp \
    datarow.cpp \
    datarowcollection.cpp \
    datatablecollection.cpp \
    shareddataobjects.cpp \
    columncollection.cpp

INCLUDEPATH += ..
