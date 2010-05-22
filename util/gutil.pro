TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    strings_private.cc \
    strings.cc \
    exception.cc \
    encryption.cc

INCLUDEPATH += headers cryptopp_src
