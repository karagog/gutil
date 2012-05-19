#-------------------------------------------------
#
# Project created by QtCreator 2012-04-22T11:03:44
#
#-------------------------------------------------

QT       -= core gui

TARGET = GUtilGames
DESTDIR = ../lib
TEMPLATE = lib

INCLUDEPATH += ../include ..
LIBS += -L../lib \
    -lGUtil


SOURCES += dice.cpp \
    cards.cpp

HEADERS += dice.h \
    cards.h


