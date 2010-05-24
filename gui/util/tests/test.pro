HEADERS += settingstest.h \
    filesystem_test.h \
    usermachinelock_test.h \
    xmlhelpers_test.h \
    flattreemodeltest.h \
    file_manager_test.h
SOURCES += settingstest.cpp \
    main.cpp \
    filesystem_test.cpp \
    usermachinelock_test.cpp \
    xmlhelpers_test.cpp \
    flattreemodeltest.cpp \
    file_manager_test.cpp
QT += testlib sql
INCLUDEPATH += ../headers \
    ../../../util/headers

win32:LIBS += \
    -lGQtUtil \
    -lgutil \
    -L../debug \
    -L../../../util/release
unix:LIBS += \
    -lGQtUtil \
    -lGstring \
    -L.. \
    -L../../../util

FORMS += \
    flattreemodeltest.ui
