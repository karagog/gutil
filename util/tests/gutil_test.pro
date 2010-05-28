HEADERS += strings_test.h \
    crypto_test.h \
    exception_test.h
SOURCES += strings_test.cpp \
    main.cpp \
    crypto_test.cpp \
    exception_test.cpp
INCLUDEPATH += ../headers

QT += testlib
QT -= gui

LIBS += -L../debug -L../cryptopp_src -lgutil -lcryptopp
