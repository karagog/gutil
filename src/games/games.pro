#-------------------------------------------------
#
# Project created by QtCreator 2012-04-22T11:03:44
#
#-------------------------------------------------

QT       -= core gui

TARGET = GUtilGames
#VERSION = 0.0.0

TOP_DIR = ../..
DESTDIR = $$TOP_DIR/lib
TEMPLATE = lib

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$DESTDIR \
    -lGUtil


SOURCES += dice.cpp \
    cards.cpp

HEADERS += dice.h \
    cards.h


