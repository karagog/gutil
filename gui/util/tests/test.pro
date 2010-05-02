HEADERS += settingstest.h \
    filesystem_test.h \
    usermachinelock_test.h \
    xmlhelpers_test.h \
    flattreemodeltest.h
SOURCES += settingstest.cpp \
    main.cpp \
    filesystem_test.cpp \
    usermachinelock_test.cpp \
    xmlhelpers_test.cpp \
    flattreemodeltest.cpp
QT += testlib
INCLUDEPATH += ../headers \
    ../../../util/strings/headers

win32:LIBS += \
    -lGQtUtilmingw \
    -L../debug \
    -L../../../util/strings/debug
unix:LIBS += \
    -lGQtUtil \
    -L.. \
    -L../..

LIBS +=  \
    -lGstring

FORMS += \
    flattreemodeltest.ui
