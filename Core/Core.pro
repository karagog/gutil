DESTDIR = ../lib

TARGET = GUtilCore

QT -= gui core

TEMPLATE = lib
#CONFIG += staticlib

CRYPTOPP_PATH = ../ThirdParty/CryptoPP

win32: CRYPTOPP_LIB = cryptopp.dll
unix:  CRYPTOPP_LIB = cryptopp

INCLUDEPATH += .. $$CRYPTOPP_PATH
LIBS += -L$$CRYPTOPP_PATH -l$$CRYPTOPP_LIB

#QMAKE_CXXFLAGS += -DGUTIL_DLL_EXPORTS
#QMAKE_CXXFLAGS += -DGUTIL_COM_EXPORTS

HEADERS += \
    exception.h \
    math.h
SOURCES += \
    exception.cc \
    math.cpp

include(BusinessObjects/BusinessObjects.pri)
include(DataAccess/DataAccess.pri)
include(Interfaces/Interfaces.pri)
include(Utils/Utils.pri)
