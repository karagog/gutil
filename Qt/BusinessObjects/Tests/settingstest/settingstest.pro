#-------------------------------------------------
#
# Project created by QtCreator 2010-10-12T21:43:43
#
#-------------------------------------------------

QT       += testlib network

TARGET = tst_settingstesttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#DEFINES += GUTIL_NO_GUI_FUNCTIONALITY

SOURCES += \
    settingstest.cpp
DEFINES += \
  SRCDIR=\\\"$$PWD/\\\" \
  DEBUG_LOGGING

HEADERS +=

INCLUDEPATH += ../../.. ../../../..
LIBS += -L../../../../lib \
    -lGUtilQt -lGUtil
