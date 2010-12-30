
DESTDIR = ../lib

TARGET = GUtilDataModels
LIBS += -L../lib \
    -lGUtilCore \
    -lGUtilInterfaces \
    -lGUtilCustom \
    -lGUtilDataObjects

TEMPLATE = lib
#CONFIG += staticlib

INCLUDEPATH += ..

HEADERS += \
    datatablemodel.h

SOURCES += \
    datatablemodel.cpp
