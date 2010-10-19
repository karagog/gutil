DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    filetransport.h \
    localsockettransport.h \
    abstractdatatransportmechanism.h \
    streamtransport.h \
    consoletransport.h \
    abstractqiodevicetransport.h

SOURCES += \
    filetransport.cpp \
    localsockettransport.cpp \
    abstractdatatransportmechanism.cpp \
    streamtransport.cpp \
    consoletransport.cpp \
    abstractqiodevicetransport.cpp

INCLUDEPATH += ..
QT += sql network
