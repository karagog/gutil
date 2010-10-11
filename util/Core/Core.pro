DESTDIR = ../lib

QT -= gui core

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    Tools/Private/strings_p.cc \
    Tools/stringhelpers.cc \
    exception.cc \
    Tools/encryption.cc \
    Utils/actionstack.cpp

INCLUDEPATH += Tools/cryptopp_src .

HEADERS += Tools/stringhelpers.h \
    exception.h \
    Tools/encryption.h \
    Utils/actionstack.h
