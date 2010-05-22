TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    strings_private.cc \
    strings.cc \
    exception.cpp \
    encryption.cc

INCLUDEPATH += headers cryptopp_src
