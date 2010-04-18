HEADERS += strings_test.h \
    crypto_test.h \
    exception_test.h
SOURCES += strings_test.cpp \
    main.cpp \
    crypto_test.cpp \
    exception_test.cpp
INCLUDEPATH += ../headers \
    ../include
QT += testlib
QT -= gui
LIBS += -L../lib
#win32:LIBS += -L../release \
win32:LIBS += -L../debug \
    -lgutil \
    -lcryptoppmingw
unix:LIBS += -L.. \
    -lgutil \
    -lcryptopp
