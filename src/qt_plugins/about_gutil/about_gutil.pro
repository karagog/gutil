
TARGET = GUtilAboutPlugin
#VERSION = 0.0.0

TEMPLATE = lib
CONFIG += plugin

TOP_DIR = ../../..
DESTDIR = $$TOP_DIR/lib

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

