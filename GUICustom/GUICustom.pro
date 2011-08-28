DESTDIR = ../lib

TARGET = GUtilGUICustom

# We need this to make use of GUI functionality of other GUtil libraries on which we are dependent
DEFINES += GUI_FUNCTIONALITY

# Switches on the use of ipc in gapplication
DEFINES += NETWORK_FUNCTIONALITY

INCLUDEPATH += ..
LIBS += -L../lib \
    -lGUtilCore \
    -lGUtilInterfaces \
    -lGUtilUtils \
    -lGUtilCustom \
    -lGUtilUi \
    -lGUtilDataObjects \
    -lGUtilBusinessObjects

TEMPLATE = lib

HEADERS += \
    gtreeview.h \
    gapplication.h \
    myflattreemodel.h \
    gundostack.h \
    gwidget.h

SOURCES += \
    gtreeview.cpp \
    gapplication.cpp \
    myflattreemodel.cpp \
    gundostack.cpp \
    gwidget.cpp
