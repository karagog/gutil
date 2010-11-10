DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    datatable.h \
    dataset.h \
    qvarianthelpers.h \
    datarow.h \
    collection.h

SOURCES += \
    datatable.cpp \
    dataset.cpp \
    qvarianthelpers.cpp \
    datarow.cpp

INCLUDEPATH += ..
