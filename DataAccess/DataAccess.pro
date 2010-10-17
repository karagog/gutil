DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    localsockettransport.h \
    abstractdatatransportmechanism.h \
    streamtransport.h \
    consoletransport.h

SOURCES += \
    ../src/qtlockedfile.cpp \
    filetransport.cpp \
    localsockettransport.cpp \
    abstractdatatransportmechanism.cpp \
    streamtransport.cpp \
    consoletransport.cpp

INCLUDEPATH += ../include ..
QT += sql network

win32 {
    SOURCES += ../src/qtlockedfile_win.cpp
}
unix {
    SOURCES += ../src/qtlockedfile_unix.cpp
}
