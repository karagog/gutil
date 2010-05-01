#-------------------------------------------------
#
# Project created by QtCreator 2010-04-20T09:58:58
#
#-------------------------------------------------

QT       -= core gui

win32:TARGET = Gstringmingw
unix:TARGET = Gstring

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += headers

SOURCES += \
    strings.cc \
    strings_private.cc

HEADERS += \
    headers/stringhelpers.h \
    strings_private.h
