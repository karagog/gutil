SOURCES += main.cpp \
    test_form.cpp
HEADERS += test_form.h
FORMS += test_form.ui

win32{
LIBS += -L../../debug -lQtControls
}
unix{
LIBS += -L../.. -lQtControls
}

INCLUDEPATH += ../../headers
