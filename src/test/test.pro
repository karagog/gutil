
TOP_DIR = ../..
QT += testlib
DESTDIR = $$TOP_DIR/lib

TARGET = GUtilTest
unix: VERSION = 0.0.0

CONFIG(debug, debug|release) {
    #message(Preparing debug build)
    DEFINES += DEBUG
}
else {
    #message(Preparing release build)
    DEFINES += QT_NO_DEBUG_OUTPUT

    # Different branches may change this line depending on where their application will deploy
    unix: QMAKE_RPATHDIR =
}

TEMPLATE = lib

HEADERS += \
    modeltest.h \
    testvectorreader.h

SOURCES += \
    modeltest.cpp

