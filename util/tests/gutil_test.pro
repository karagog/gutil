HEADERS += strings_test.h \
    crypto_test.h \
    exception_test.h
SOURCES += strings_test.cpp \
    main.cpp \
    crypto_test.cpp \
    exception_test.cpp
INCLUDEPATH += ../encryption/headers ../strings/headers ../exceptions/headers

QT += testlib
QT -= gui
LIBS += -L../encryption/debug -L../strings/debug -L../exceptions/debug -L../encryption/lib \
    -lGexcept
#win32:LIBS += -L../release \
win32:LIBS += -L../debug \
    -lGstringmingw \
    -lGencryptmingw \
    -lcryptoppmingw
unix:LIBS += -L.. \
    -lGstring \
    -lGencrypt \
    -lcryptopp
