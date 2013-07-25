QT -= gui core
DESTDIR = ../../lib
TARGET = apfloat
TEMPLATE = lib

CONFIG += staticlib
CONFIG += warn_off

INCLUDEPATH +=
QMAKE_CXXFLAGS =
QMAKE_CXXFLAGS += -lm -ffast-math -DBIN=binary

OTHER_FILES += \
    legal.txt

HEADERS += \

SOURCES += \
    aprfunc.cpp \
    aprat.cpp \
    apifunc.cpp \
    apint.cpp \
    tapfunc.cpp \
    tapfloat.cpp \
    tapstruc.cpp \
    primetab.cpp \
    prime.cpp \
    modfunc.cpp \
    bigint.cpp \
    move.cpp \
    scramble.cpp \
    tablefnt.cpp \
    tabletwt.cpp \
    tablesit.cpp \
    transpos.cpp \
    file.cpp \
    global.cpp \
    init.cpp \
    carrycrt.cpp \
    convolut.cpp \
    apcfunc.cpp \
    apcplx.cpp \
    apfunc.cpp \
    baseint.cpp \
    apstruct.cpp \
    apfloat.cpp \
    datastru.cpp \
    truncate.c
