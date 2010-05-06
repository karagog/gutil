# -------------------------------------------------
# Project created by QtCreator 2010-03-27T16:39:45
# -------------------------------------------------
win32:TARGET = GQtUtilmingw
unix:TARGET = GQtUtil
TEMPLATE = lib
CONFIG += staticlib
SOURCES += settings.cpp \
    widgethelpers.cpp \
    src/qtlockedfile.cpp \
    filesystemhelpers.cpp \
    usermachinelock.cpp \
    xmlhelpers.cpp \
    myflattreemodel.cpp
HEADERS += headers/settings.h \
    headers/widgethelpers.h \
    headers/filesystemhelpers.h \
    headers/usermachinelock.h \
    headers/xmlhelpers.h \
    headers/myflattreemodel.h
INCLUDEPATH += headers \
    ../../util/strings/headers \
    include

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
