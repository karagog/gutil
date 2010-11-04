DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    datatable.h \
    dataset.h \
    qvarianthelpers.h

SOURCES += \
    datatable.cpp \
    dataset.cpp \
    qvarianthelpers.h

INCLUDEPATH += ..
