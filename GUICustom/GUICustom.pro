DESTDIR = ../lib

TARGET = GUtilGUICustom

INCLUDEPATH += ..
LIBS += -L../lib \
    -lGUtilCore \
    -lGUtilInterfaces \
    -lGUtilUtils \
    -lGUtilCustom

TEMPLATE = lib

HEADERS += \
    gtreeview.h \
    gapplication.h \
    myflattreemodel.h

SOURCES += \
    gtreeview.cpp \
    gapplication.cpp \
    myflattreemodel.cpp
