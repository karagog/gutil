#-------------------------------------------------
#
# Project created by QtCreator 2010-10-12T21:26:46
#
#-------------------------------------------------

QT       += testlib sql network

TARGET = file_manager_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += file_manager_test.cpp
#tst_filemanagertest.cpp \
DEFINES += SRCDIR=\\\"$$PWD/\\\" \
            DEBUG_LOGGING

#HEADERS +=

INCLUDEPATH += ../../../../include
LIBS += -L../../../../lib \
    -lGUtil \
    -lGUtilQt \
    -lcryptopp
