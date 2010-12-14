DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    myflattreemodel.h \
    gthread.h \
    gsemaphore.h \
    gsharedlock.h \
    gvariant.h \
    gshareddata.h \
    updatablegvariant.h \
    signalwaitcondition.h

SOURCES += \
    myflattreemodel.cpp \
    gthread.cpp \
    gsemaphore.cpp \
    gsharedlock.cpp \
    gvariant.cpp \
    updatablegvariant.cpp \
    signalwaitcondition.cpp

INCLUDEPATH += ..
