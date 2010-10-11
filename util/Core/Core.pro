DESTDIR = ../lib

QT -= gui core

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    Tools/Private/strings_p.cc \
    Tools/stringhelpers.cc \
    exception.cc \
    Tools/encryption.cc \
    Utils/actionstack.cpp \
    Tools/cl_menu.cc \
    DataAccess/da_serialport.cc

INCLUDEPATH += Tools/cryptopp_src .

HEADERS += Tools/stringhelpers.h \
    exception.h \
    Tools/encryption.h \
    Utils/actionstack.h \
    Tools/cl_menu.h \
    DataAccess/da_serialport.h
