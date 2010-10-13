DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    UserMachineLock.h \
    BinaryDataStore.h \
    LocalClientServer.h \
    ConfigFile.h \
    Private/ValueBuffer.h \
    Private/Transports/filetransport.h \
    Private/io_queue_worker.h \
    Private/Transports/localsockettransport.h

SOURCES += \
    UserMachineLock.cpp \
    BinaryDataStore.cpp \
    src/qtlockedfile.cpp \
    LocalClientServer.cpp \
    ConfigFile.cpp \
    Private/ValueBuffer.cpp \
    Private/Transports/filetransport.cpp \
    Private/io_queue_worker.cpp \
    Private/Transports/localsockettransport.cpp

INCLUDEPATH += include ..
QT += sql network

win32 {
    SOURCES += src/qtlockedfile_win.cpp
}
unix {
    SOURCES += src/qtlockedfile_unix.cpp
}
