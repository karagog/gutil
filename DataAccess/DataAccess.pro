DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    UserMachineLock.h \
    BinaryDataStore.h \
    LocalClientServer.h \
    ConfigFile.h \
    abstractvaluebuffer.h \
    DataTransports/filetransport.h \
    io_queue_worker.h \
    DataTransports/localsockettransport.h \
    DataTransports/abstractdatatransportmechanism.h \
    DataTransports/streamtransport.h \
    DataTransports/consoletransport.h

SOURCES += \
    UserMachineLock.cpp \
    BinaryDataStore.cpp \
    src/qtlockedfile.cpp \
    LocalClientServer.cpp \
    ConfigFile.cpp \
    abstractvaluebuffer.cpp \
    DataTransports/filetransport.cpp \
    io_queue_worker.cpp \
    DataTransports/localsockettransport.cpp \
    DataTransports/abstractdatatransportmechanism.cpp \
    DataTransports/streamtransport.cpp \
    DataTransports/consoletransport.cpp

INCLUDEPATH += include ..
QT += sql network

win32 {
    SOURCES += src/qtlockedfile_win.cpp
}
unix {
    SOURCES += src/qtlockedfile_unix.cpp
}
