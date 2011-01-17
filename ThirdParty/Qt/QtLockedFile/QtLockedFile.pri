
HEADERS += \
    QtLockedFile/qtlockedfile.h

SOURCES += \
    QtLockedFile/qtlockedfile.cpp

win32 {
    SOURCES += QtLockedFile/qtlockedfile_win.cpp
}
unix {
    SOURCES += QtLockedFile/qtlockedfile_unix.cpp
}
