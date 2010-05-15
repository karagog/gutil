#-------------------------------------------------
#
# Project created by QtCreator 2010-04-20T10:03:05
#
#-------------------------------------------------

QT       -= core gui

TARGET = Gencrypt

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += cryptopp_src headers

SOURCES += \
    encryption.cc

HEADERS += \
    headers/encryption.h
