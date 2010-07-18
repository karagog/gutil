DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    DA_UserMachineLock.h \
    DA_BinaryDataStore.h \
    DA_LocalClientServer.h \
    DA_ConfigFile.h \
    Private/DA_ValueBuffer_P.h \
    Private/Transports/ITransportMechanism.h \
    Private/Transports/filetransport.h \
    Private/Transports/sockettransport.h

SOURCES += \
    DA_UserMachineLock.cpp \
    DA_BinaryDataStore.cpp \
    src/qtlockedfile.cpp \
    DA_LocalClientServer.cpp \
    DA_ConfigFile.cpp \
    Private/DA_ValueBuffer_P.cpp \
    Private/Transports/filetransport.cpp \
    Private/Transports/sockettransport.cpp \
    Private/Transports/ITransportMechanism.cpp

INCLUDEPATH += ../../util include ..
QT += sql network

win32 {
    SOURCES += src/qtlockedfile_win.cpp
}
unix {
    SOURCES += src/qtlockedfile_unix.cpp
}
