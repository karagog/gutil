DESTDIR = ../lib

QT -= gui core

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    exception.cc

INCLUDEPATH += .. ../ThirdParty/CryptoPP

HEADERS += \
    exception.h

include(BusinessObjects/BusinessObjects.pri)
include(DataAccess/DataAccess.pri)
include(Interfaces/Interfaces.pri)
include(Utils/Utils.pri)
