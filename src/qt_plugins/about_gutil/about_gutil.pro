
QT += widgets

TARGET = GUtilAboutPlugin

CONFIG(debug, debug|release) {
    #message(Preparing debug build)
    DEFINES += DEBUG
}
else {
    #message(Preparing release build)

    # Different branches may change this line depending on where their application will deploy
    unix: QMAKE_RPATHDIR = /usr/local/lib/gryptonite
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

OTHER_FILES +=

win32{
    copy_license.commands = copy ..\..\..\LICENSE license.txt
    copy_notice.commands = copy  ..\..\..\NOTICE notice.txt
}
unix{
    copy_license.commands = cp ../../../LICENSE license.txt
    copy_notice.commands =  cp ../../../NOTICE notice.txt
}

PRE_TARGETDEPS = \
    copy_license \
    copy_notice

QMAKE_EXTRA_TARGETS = \
    copy_license \
    copy_notice
