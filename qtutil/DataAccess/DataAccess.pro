DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    DA_UserMachineLock.h \
    DA_BinaryDataStore.h \
    DA_LocalClientServer.h \
    DA_ConfigFile.h \
    Private/ValueBuffer.h \
    Private/filetransport.h \
    Private/io_device_transport.h \
    Private/io_queue_worker.h \
    Private/localsockettransport.h

SOURCES += \
    DA_UserMachineLock.cpp \
    DA_BinaryDataStore.cpp \
    src/qtlockedfile.cpp \
    DA_LocalClientServer.cpp \
    DA_ConfigFile.cpp \
    Private/ValueBuffer.cpp \
    Private/filetransport.cpp \
    Private/io_device_transport.cpp \
    Private/io_queue_worker.cpp \
    Private/localsockettransport.cpp

INCLUDEPATH += ../../util include ..
QT += sql network

win32 {
    SOURCES += src/qtlockedfile_win.cpp
}
unix {
    SOURCES += src/qtlockedfile_unix.cpp
}
