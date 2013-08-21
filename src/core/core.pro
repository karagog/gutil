
TARGET = GUtil
#VERSION = 0.0.0

QT -= gui core

TEMPLATE = lib
#CONFIG += staticlib

TOP_DIR = ../..

DESTDIR = $$TOP_DIR/lib

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$TOP_DIR/lib \
    -lcryptopp

#DEFINES += GUTIL_COM_EXPORTS
#DEFINES += GUTIL_NO_CRYPTOPP
#DEFINES += GUTIL_DEBUG

HEADERS += \
    exception.h \
    extendedexception.h \
    globals.h \
    macros.h
SOURCES += \
    extendedexception.cpp \
    exception.cpp \
    globals.cpp

include(DataAccess/DataAccess.pri)
include(DataObjects/DataObjects.pri)
include(Interfaces/Interfaces.pri)
include(Logging/Logging.pri)
include(Utils/Utils.pri)










