#-------------------------------------------------
#
# Project created by QtCreator 2012-04-22T11:03:44
#
#-------------------------------------------------

QT       -= core gui

TARGET = GUtilGames
unix: VERSION = 0.0.0

TOP_DIR = ../..
DESTDIR = $$TOP_DIR/lib
TEMPLATE = lib
CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

CONFIG(debug, debug|release) {
    #message(Preparing debug build)
    DEFINES += DEBUG
}
else {
    #message(Preparing release build)

    # Different branches may change this line depending on where their application will deploy
    unix: QMAKE_RPATHDIR =
}

INCLUDEPATH += $$TOP_DIR/include
#LIBS += -L$$DESTDIR -lGUtil


SOURCES += dice.cpp \
    cards.cpp

HEADERS += dice.h \
    cards.h


