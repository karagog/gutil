
QT -= gui core

TARGET = cryptest
TEMPLATE = app
CONFIG += console

QMAKE_CXXFLAGS += -pipe -w

win32: QMAKE_CXXFLAGS += -mthreads

DEFINES += NDEBUG
DEFINES -= UNICODE

DEFINES += CRYPTOPP_EXPORTS

INCLUDEPATH += ..
LIBS += -L../../../lib -lcryptopp -lws2_32

SOURCES += \
    test.cpp \
    regtest.cpp \
    fipstest.cpp \
    validat3.cpp \
    validat2.cpp \
    validat1.cpp \
    fipsalgt.cpp \
    bench.cpp \
    bench2.cpp \
    datatest.cpp

