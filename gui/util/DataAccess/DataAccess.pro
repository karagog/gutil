DESTDIR = ../lib

TEMPLATE = lib
CONFIG += staticlib

HEADERS += \
    DA_UserMachineLock.h \
    DA_PersistentSettings.h \
    DA_BinaryDataStore.h

SOURCES += \
    DA_UserMachineLock.cpp \
    DA_PersistentSettings.cpp \
    DA_BinaryDataStore.cpp

INCLUDEPATH += ../../../util include
QT += sql

win32 {
    SOURCES += src/qtlockedfile_win.cpp
}
unix {
    SOURCES += src/qtlockedfile_unix.cpp
}
