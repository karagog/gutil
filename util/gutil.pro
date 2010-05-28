TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    strings_private.cc \
    stringhelpers.cc \
    exception.cc \
    encryption.cc

INCLUDEPATH += headers cryptopp_src

HEADERS += headers/stringhelpers.h \
    headers/exception.h \
    headers/encryption.h
