#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T18:47:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = select_date_range
TEMPLATE = app

INCLUDEPATH += ../../../../../include
LIBS += -L../../../../../lib -lGUtilQt -lGUtil

DEFINES += DEBUG


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h
