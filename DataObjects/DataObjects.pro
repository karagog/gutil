DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    datatable.h \
    dataset.h

SOURCES += \
    datatable.cpp \
    dataset.cpp

INCLUDEPATH += ..
