#-------------------------------------------------
#
# Project created by QtCreator 2014-05-15T20:18:37
#
#-------------------------------------------------

QT       -= gui

TARGET = GUtilCryptoPP
TEMPLATE = lib
CONFIG += staticlib

TOP_DIR = ../..
DESTDIR = $$TOP_DIR/lib

INCLUDEPATH += $$TOP_DIR/include

CONFIG(debug, debug|release) {
    #message(Preparing debug build)
    DEFINES += DEBUG
}
else {
    #message(Preparing release build)
}

SOURCES += \
    cryptopp_rng.cpp \
    cryptopp_sinks.cpp \
    cryptopp_utils.cpp

HEADERS += \
    cryptopp_rng.h \
    cryptopp_sinks.h \
    cryptopp_utils.h
