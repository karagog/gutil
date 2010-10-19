DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    filetransport.h \
    localsockettransport.h \
    abstractdatatransportmechanism.h \
    streamtransport.h \
    consoletransport.h \
    abstractqiodevicetransport.h \
    processtransport.h

SOURCES += \
    filetransport.cpp \
    localsockettransport.cpp \
    abstractdatatransportmechanism.cpp \
    streamtransport.cpp \
    consoletransport.cpp \
    abstractqiodevicetransport.cpp \
    processtransport.cpp

INCLUDEPATH += ..
QT += sql network
