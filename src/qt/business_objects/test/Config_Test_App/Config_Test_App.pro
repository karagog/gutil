#-------------------------------------------------
#
# Project created by QtCreator 2010-12-12T16:38:30
#
#-------------------------------------------------

QT       += core gui

TARGET = Config_Test_App
TEMPLATE = app

#DEFINES += GUTIL_NO_GUI_FUNCTIONALITY

TOP_DIR = ../../../../..

INCLUDEPATH += $$TOP_DIR/include
LIBS += \
    -L$$TOP_DIR/lib \
    -lGUtil \
    -lGUtilQt

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
