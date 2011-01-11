DESTDIR = ../lib

TARGET = GUtilCore

QT -= gui core

TEMPLATE = lib
#CONFIG += staticlib

CRYPTOPP_PATH = ../ThirdParty/CryptoPP

win32: CRYPTOPP_LIB = cryptopp
unix:  CRYPTOPP_LIB = cryptopp

INCLUDEPATH += .. $$CRYPTOPP_PATH
LIBS += -L$$CRYPTOPP_PATH -l$$CRYPTOPP_LIB

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
