#-------------------------------------------------
#
# Project created by QtCreator 2010-12-29T21:27:13
#
#-------------------------------------------------

QT       += core gui

TARGET = DataTableModel
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../..
LIBS += -L../../../lib \
    -lGUtilInterfaces \
    -lGUtilCustom \
    -lGUtilDataModels \
    -lGUtilDataObjects
