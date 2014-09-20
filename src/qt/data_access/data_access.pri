
HEADERS += \
    data_access/iodevice.h \
    data_access/qt_iodevice.h \
    data_access/consoleiodevice.h \
    data_access/databaseiodevice.h \
    data_access/fileiodevice.h \
    data_access/localsocketiodevice.h \
    $$PWD/qt_settings.h

SOURCES += \
    data_access/iodevice.cpp \
    data_access/qt_iodevice.cpp \
    data_access/consoleiodevice.cpp \
    data_access/localsocketiodevice.cpp \
    data_access/fileiodevice.cpp \
    data_access/databaseiodevice.cpp \
    $$PWD/qt_settings.cpp




