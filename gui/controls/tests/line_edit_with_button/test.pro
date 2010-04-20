SOURCES += main.cpp \
    test_form.cpp
HEADERS += test_form.h
FORMS += test_form.ui

win32{
LIBS += -L../../debug -lQtControlsmingw
}
unix{
LIBS += -L../.. -lQtControls
}

INCLUDEPATH += ../../headers
