DESTDIR = ../lib

TARGET = GUtilCore

QT -= gui core

TEMPLATE = lib
#CONFIG += staticlib

INCLUDEPATH += .. ../ThirdParty/CryptoPP
LIBS += -L../lib -lcryptopp

#QMAKE_CXXFLAGS += -DGUTIL_DLL_EXPORTS
#QMAKE_CXXFLAGS += -DGUTIL_COM_EXPORTS

HEADERS += \
    exception.h \
    math.h
SOURCES += \
    exception.cc \
    math.cpp

include(DataAccess/DataAccess.pri)
include(Interfaces/Interfaces.pri)
include(Utils/Utils.pri)
