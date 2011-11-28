DESTDIR = ../lib

TARGET = GUtilCore

QT -= gui core

TEMPLATE = lib
#CONFIG += staticlib

DEFINES += GUTIL_ENCRYPTION

#DEFINES += DEBUG

INCLUDEPATH += .. ../ThirdParty
LIBS += -L../lib -lcryptopp

#QMAKE_CXXFLAGS += -DGUTIL_DLL_EXPORTS
#QMAKE_CXXFLAGS += -DGUTIL_COM_EXPORTS

HEADERS += \
    exception.h \
    extendedexception.h \
    gutil_globals.h
SOURCES += \
    globals.cpp \
    extendedexception.cpp \
    exception.cpp

include(DataAccess/DataAccess.pri)
include(DataObjects/DataObjects.pri)
include(Interfaces/Interfaces.pri)
include(Utils/Utils.pri)








