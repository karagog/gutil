DESTDIR = ../lib

TARGET = GUtil

QT -= gui core

TEMPLATE = lib
#CONFIG += staticlib

INCLUDEPATH += ../include .. ../ThirdParty/fdlibm-5.2
LIBS += -L../lib \
    -lcryptopp

#DEFINES += GUTIL_COM_EXPORTS
#DEFINES += GUTIL_NO_CRYPTOPP

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










