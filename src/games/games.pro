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

build_pass:CONFIG(debug, debug|release) {
    message(Preparing debug build)
    DEFINES += DEBUG
}
else:build_pass {
    message(Preparing release build)
}

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$DESTDIR \
    -lGUtil


SOURCES += dice.cpp \
    cards.cpp

HEADERS += dice.h \
    cards.h


