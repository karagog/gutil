DESTDIR = ../lib

TARGET = GUtilCustom

LIBS += -L../lib -lGUtilCore -lGUtilInterfaces -lGUtilUtils

TEMPLATE = lib
#CONFIG += staticlib

HEADERS += \
    myflattreemodel.h \
    gthread.h \
    gsemaphore.h \
    gsharedlock.h \
    gvariant.h \
    gshareddata.h \
    updatablegvariant.h \
    gpairlist.h \
    gtreeview.h \
    gdatetime.h \
    gformattedtext.h \
    gapplication.h

SOURCES += \
    myflattreemodel.cpp \
    gthread.cpp \
    gsharedlock.cpp \
    gvariant.cpp \
    updatablegvariant.cpp \
    gtreeview.cpp \
    gdatetime.cpp \
    gapplication.cpp

INCLUDEPATH += ..
