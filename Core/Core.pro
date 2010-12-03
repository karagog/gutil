DESTDIR = ../lib

QT -= gui core

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    Utils/Private/strings_p.cc \
    Utils/stringhelpers.cc \
    exception.cc \
    Utils/encryption.cc \
    BusinessObjects/actionstack.cpp \
    Utils/cl_menu.cc \
    DataAccess/da_serialport.cc \
    Interfaces/ireadonlyobject.cpp

INCLUDEPATH += .. ../ThirdParty/CryptoPP

HEADERS += \
    Utils/stringhelpers.h \
    exception.h \
    Utils/encryption.h \
    BusinessObjects/actionstack.h \
    Utils/cl_menu.h \
    DataAccess/da_serialport.h \
    Interfaces/ireadonlyobject.h \
    Interfaces/ixmlserializable.h \
    Interfaces/istringable.h \
    Interfaces/iupdatable.h \
    Interfaces/icomparable.h \
    Interfaces/icomparer.h \
    Interfaces/iequatable.h \
    Interfaces/iequalitycomparer.h \
    Interfaces/iclonable.h \
    Interfaces/iconvertable.h \
    Interfaces/ivalueobserver.h \
    Interfaces/iobservablevalue.h
