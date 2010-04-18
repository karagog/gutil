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
    ../../headers

# Need to declare these libraries before cryptopp
LIBS += -lGQtUtil \
    -lgutil
win32:LIBS += -L../debug \
    -L../../release \
    -L../../lib \
    -lcryptoppmingw
unix:LIBS += -L.. \
    -L../.. \
    -L../../lib \
    -lcryptopp \
    -Wl,-rpath,..
