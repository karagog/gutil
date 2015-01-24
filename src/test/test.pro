
TOP_DIR = ../..
QT += testlib
DESTDIR = $$TOP_DIR/lib

TARGET = GUtilTest
unix: VERSION = 0.0.0

# Different branches may change this line depending on where their application will deploy
unix: QMAKE_RPATHDIR =

TEMPLATE = lib

HEADERS += \
    modeltest.h \
    testvectorreader.h

SOURCES += \
    modeltest.cpp

