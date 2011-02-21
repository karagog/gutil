# -------------------------------------------------
# Project created by QtCreator 2010-04-17T23:08:47
# -------------------------------------------------
QT += network
DESTDIR = ../lib

# If you want database functionality, uncomment these
#QT += sql
#DEFINES += DATABASE_FUNCTIONALITY

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
    processstatusindicator.h

SOURCES += \
    BinaryDataStore.cpp \
    ConfigFile.cpp \
    giodevicebundlemanager.cpp \
    localsocketserver.cpp \
    localsocketclient.cpp \
    processstatusindicator.cpp

INCLUDEPATH += ..
