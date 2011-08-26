DESTDIR = ../lib

TARGET = GUtilGUICustom

INCLUDEPATH += ..
LIBS += -L../lib \
    -lGUtilCore \
    -lGUtilInterfaces \
    -lGUtilUtils \
    -lGUtilCustom \
    -lGUtilUI

TEMPLATE = lib

HEADERS += \
    gtreeview.h \
    gapplication.h \
    myflattreemodel.h \
    gundostack.h \
    gwidget.h

SOURCES += \
    gtreeview.cpp \
    gapplication.cpp \
    myflattreemodel.cpp \
    gundostack.cpp \
    gwidget.cpp
