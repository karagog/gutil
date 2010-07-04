# -------------------------------------------------
# Project created by QtCreator 2010-03-27T16:39:45
# -------------------------------------------------
QT += sql
TARGET = GQtUtil
TEMPLATE = lib
CONFIG += staticlib
SOURCES += settings.cpp \
    widgethelpers.cpp \
    src/qtlockedfile.cpp \
    filesystemhelpers.cpp \
    usermachinelock.cpp \
    xmlhelpers.cpp \
    myflattreemodel.cpp \
    file_manager.cpp \
    cancellablethread.cpp \
    pubsubsystem.cpp
HEADERS += settings.h \
    widgethelpers.h \
    filesystemhelpers.h \
    usermachinelock.h \
    xmlhelpers.h \
    myflattreemodel.h \
    file_manager.h \
    cancellablethread.h \
    pubsubsystem.h
INCLUDEPATH += ../../util include

# For static builds
# LIBS += -lgutil
win32 {
    LIBS += -L../release
    SOURCES += src/qtlockedfile_win.cpp
}
unix {
    LIBS += -L..
    SOURCES += src/qtlockedfile_unix.cpp
}
