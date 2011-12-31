#-------------------------------------------------
#
# Project created by QtCreator 2010-11-04T23:04:32
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_datasettest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_datasettest.cpp \
    dogtable.cpp \
    peopletable.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../.. ../../../..
LIBS += -L../../../../lib \
    -lGUtil \
    -lGUtil \
    -lGUtilThirdPartyQtLockedFile

unix: LIBS += -Wl,-rpath,../../../lib

DEFINES += \
    DEBUG_LOGGING \
    #GUI_FUNCTIONALITY

HEADERS += \
    persondatarow.h \
    peopletable.h \
    dogrow.h \
    dogtable.h \
    dogownerdataset.h
