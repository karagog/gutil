
QT += widgets

TARGET = GUtilAboutPlugin

CONFIG(debug, debug|release) {
    #message(Preparing debug build)
    DEFINES += DEBUG
}
else {
    #message(Preparing release build)

    # Different branches may change this line depending on where their application will deploy
    unix: QMAKE_RPATHDIR =
}

TEMPLATE = lib
CONFIG += plugin

TOP_DIR = ../../..
DESTDIR = $$TOP_DIR/lib

QMAKE_CXXFLAGS += -std=c++11

#DEFINES += GUTIL_NO_GUI_FUNCTIONALITY

INCLUDEPATH += $$TOP_DIR/include
LIBS += -L$$DESTDIR \
    -lGUtil \
    -lGUtilQt

SOURCES += aboutgutil.cpp

HEADERS += aboutgutil.h

RESOURCES += \
    aboutgutil.qrc

OTHER_FILES += \
    abouttext.txt \
    licensetext.txt

