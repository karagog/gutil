DESTDIR = ../lib
QT -= gui

TARGET = GUtilCustom

LIBS += -L../lib \
    -lGUtilCore \
    -lGUtilInterfaces \
    -lGUtilUtils

DEFINES += GUTIL_CORE_QT_ADAPTERS

TEMPLATE = lib
#CONFIG += staticlib

HEADERS += \
    gthread.h \
    gsemaphore.h \
    gsharedlock.h \
    gvariant.h \
    updatablegvariant.h \
    gpairlist.h \
    gdatetime.h \
    gformattedtext.h \
    gcoreapplication.h \
    gapplicationbase.h

SOURCES += \
    gthread.cpp \
    gsharedlock.cpp \
    gvariant.cpp \
    updatablegvariant.cpp \
    gdatetime.cpp \
    gcoreapplication.cpp \
    gapplicationbase.cpp

INCLUDEPATH += ..
