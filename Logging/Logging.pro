# Uncomment these to enable database functionality
#QT += sql
#DEFINES += DATABASE_FUNCTIONALITY

# Un/Comment these if you need GUI functionality
QT -= gui
#DEFINES += GUI_FUNCTIONALITY

DESTDIR = ../lib

TARGET = GUtilLogging
LIBS += -L../lib \
    -lGUtilCore \
    -lGUtilUtils \
    -lGUtilDataAccess

TEMPLATE = lib
#CONFIG += staticlib

INCLUDEPATH += ..

HEADERS += \
    globallogger.h \
    filelogger.h \
    consolelogger.h \
    abstractlogger.h \
    abstractdatabaselogger.h \
    messageboxlogger.h \
    grouplogger.h \
    debuglogger.h

SOURCES += \
    globallogger.cpp \
    filelogger.cpp \
    consolelogger.cpp \
    abstractlogger.cpp \
    abstractdatabaselogger.cpp \
    messageboxlogger.cpp \
    grouplogger.cpp \
    debuglogger.cpp

DEFINES += DEBUG_LOGGING
