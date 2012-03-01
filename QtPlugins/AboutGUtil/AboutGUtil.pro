
TARGET = GUtilAboutPlugin
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../lib

DEFINES += \
    GUI_FUNCTIONALITY

INCLUDEPATH += ../../include
LIBS += -L../../lib \
    -lGUtil \
    -lGUtilQt

SOURCES += aboutgutil.cpp

HEADERS += aboutgutil.h

RESOURCES += \
    aboutgutil.qrc

OTHER_FILES += \
    abouttext.txt

