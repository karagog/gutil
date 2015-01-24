TARGET = GUtil
unix: VERSION = 0.0.0

# Different branches may change this line depending on where their application will deploy
unix: QMAKE_RPATHDIR = /usr/local/lib/gryptonite

QT -= gui core

TEMPLATE = lib
#CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

CONFIG += warn_off

TOP_DIR = ../..

DESTDIR = $$TOP_DIR/lib

INCLUDEPATH += $$TOP_DIR/include
#LIBS += -L$$TOP_DIR/lib

# Enables STL dependent functions
#DEFINES += GUTIL_STL

# Disables the default global rng.
#  Don't do this unless you know what you're doing
#DEFINES += GUTIL_NO_DEFAULT_RNG

#DEFINES += GUTIL_COM_EXPORTS

CONFIG(debug, debug|release) {
    #message(Preparing debug build)
    DEFINES += DEBUG
}
else {
    #message(Preparing release build)
}


HEADERS += \
    exception.h \
    globals.h \
    macros.h
SOURCES += \
    globals.cpp

include(math/math.pri)
include(data_access/data_access.pri)
include(data_objects/data_objects.pri)
include(interfaces/interfaces.pri)
include(logging/logging.pri)
include(utils/utils.pri)










