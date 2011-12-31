DESTDIR = ../lib

TARGET = GUtilCryptoPP

QT -= gui core

TEMPLATE = lib
#CONFIG += staticlib

#DEFINES += DEBUG

INCLUDEPATH += . .. ../ThirdParty
LIBS += -L../lib -lGUtil -lcryptopp

#QMAKE_CXXFLAGS += -DGUTIL_DLL_EXPORTS
#QMAKE_CXXFLAGS += -DGUTIL_COM_EXPORTS

HEADERS += \
    encryptionutils.h \
    gstringsink.h
SOURCES += \
    encryptionutils.cpp

#include(DataAccess/DataAccess.pri)







