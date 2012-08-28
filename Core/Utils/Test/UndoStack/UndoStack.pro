#-------------------------------------------------
#
# Project created by QtCreator 2012-08-28T17:01:39
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_undostacktest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_undostacktest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

LIBS += -L../../../../lib \
    -lGUtil

INCLUDEPATH += ../../../../include

unix{

LIBS += \
    -lcryptopp
}
