
QT -= gui
DESTDIR = ../../lib
TARGET = GUtilThirdPartyQtLockedFile

TEMPLATE = lib
CONFIG += staticlib
CONFIG += warn_off

HEADERS += \
    qtlockedfile.h

SOURCES += \
    qtlockedfile.cpp

win32: SOURCES += qtlockedfile_win.cpp
unix:  SOURCES += qtlockedfile_unix.cpp

