
TOP_DIR = ../..
QT += testlib
DESTDIR = $$TOP_DIR/lib

TARGET = GUtilTest
unix: VERSION = 0.0.0

TEMPLATE = lib

HEADERS += \
    modeltest.h \
    testvectorreader.h

SOURCES += \
    modeltest.cpp

