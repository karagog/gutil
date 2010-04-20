HEADERS += settingstest.h \
    filesystem_test.h \
    usermachinelock_test.h \
    xmlhelpers_test.h
SOURCES += settingstest.cpp \
    main.cpp \
    filesystem_test.cpp \
    usermachinelock_test.cpp \
    xmlhelpers_test.cpp
QT += testlib
INCLUDEPATH += ../headers \
    ../../../util/strings/headers

# Need to declare these libraries before cryptopp
LIBS += -lGQtUtil \
    -lGstring
win32:LIBS += -L../debug \
    -L../../../util/strings/debug
unix:LIBS += -L.. \
    -L../..
