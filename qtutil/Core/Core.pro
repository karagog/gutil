DESTDIR = ../lib

QT -= gui core

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    strings_private.cc \
    stringhelpers.cc \
    exception.cc \
    encryption.cc \
    actionstack.cpp

INCLUDEPATH += cryptopp_src

HEADERS += stringhelpers.h \
    exception.h \
    encryption.h \
    actionstack.h
