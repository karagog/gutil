DESTDIR = ../lib

TARGET = GUtil

QT -= gui core

TEMPLATE = lib
#CONFIG += staticlib

#DEFINES += GUTIL_CRYPTOPP

#DEFINES += DEBUG

INCLUDEPATH += . ..

#QMAKE_CXXFLAGS += -DGUTIL_DLL_EXPORTS
#QMAKE_CXXFLAGS += -DGUTIL_COM_EXPORTS

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










