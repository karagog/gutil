
DESTDIR = ../lib

TARGET = GUtilModelViews
TEMPLATE = lib

INCLUDEPATH += ..
LIBS += -L../lib \
    -lGUtilCustom \
    -lGUtilCore

HEADERS += \
    timelineview.h

SOURCES += \
    timelineview.cpp
