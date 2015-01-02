#-------------------------------------------------
#
# Project created by QtCreator 2014-05-15T20:18:37
#
#-------------------------------------------------

QT       -= gui

TARGET = GUtilCryptoPP
TEMPLATE = lib
#CONFIG += staticlib
QMAKE_CXXFLAGS += -std=c++11

TOP_DIR = ../..
DESTDIR = $$TOP_DIR/lib

INCLUDEPATH += $$TOP_DIR/include

win32{
LIBS += \
    -L$$TOP_DIR/lib \
    -lGUtil \
    -lcryptopp
}

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
    cryptopp_cryptor.cpp \
    cryptopp_hash.cpp \
    gpsutils.cpp

HEADERS += \
    cryptopp_rng.h \
    cryptopp_sinks.h \
    cryptopp_cryptor.h \
    cryptopp_hash.h \
    gpsutils.h
