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
    ../src/qtlockedfile.cpp \
    filetransport.cpp \
    localsockettransport.cpp \
    abstractdatatransportmechanism.cpp \
    streamtransport.cpp \
    consoletransport.cpp \
    abstractqiodevicetransport.cpp

INCLUDEPATH += ../include ..
QT += sql network

win32 {
    SOURCES += ../src/qtlockedfile_win.cpp
}
unix {
    SOURCES += ../src/qtlockedfile_unix.cpp
}
