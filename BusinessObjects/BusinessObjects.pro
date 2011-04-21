# -------------------------------------------------
# Project created by QtCreator 2010-04-17T23:08:47
# -------------------------------------------------
DESTDIR = ../lib

# If you want database functionality, uncomment these
#QT += sql
#DEFINES += DATABASE_FUNCTIONALITY

#QT += network
#DEFINES += NETWORK_FUNCTIONALITY

QT -= gui
#DEFINES += GUI_FUNCTIONALITY

TARGET = GUtilBusinessObjects
LIBS += -L../lib \
    -lGUtilCore \
    -lGUtilInterfaces \
    -lGUtilUtils \
    -lGUtilCustom \
    -lGUtilDataObjects \
    -lGUtilDataAccess \
    -lGUtilLogging

TEMPLATE = lib
#CONFIG += staticlib
HEADERS += \
    BinaryDataStore.h \
    ConfigFile.h \
    giodevicebundlemanager.h \
    localsocketserver.h \
    localsocketclient.h \
    processstatusindicator.h \
    connectionmanager.h

SOURCES += \
    BinaryDataStore.cpp \
    ConfigFile.cpp \
    giodevicebundlemanager.cpp \
    localsocketserver.cpp \
    localsocketclient.cpp \
    processstatusindicator.cpp \
    connectionmanager.cpp

INCLUDEPATH += ..
