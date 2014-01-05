
TARGET = GUtilQt
#VERSION = 0.0.0

TEMPLATE = lib

#QT -= gui
#DEFINES += GUTIL_NO_GUI_FUNCTIONALITY

QT += sql
#DEFINES += GUTIL_NO_DATABASE_FUNCTIONALITY

QT += network
#DEFINES += GUTIL_NO_NETWORK_FUNCTIONALITY

DEFINES += DEBUG

# So we can interface our core classes more easily with Qt
DEFINES += QT_ADAPTERS

# Enables STL dependent functions
#DEFINES += GUTIL_STL

TOP_DIR = ../..

DESTDIR = $$TOP_DIR/lib

INCLUDEPATH += $$TOP_DIR/include ../third_party
LIBS += -L$$DESTDIR -lGUtil

include(interfaces/interfaces.pri)
include(utils/utils.pri)
include(custom/custom.pri)
include(ui/ui.pri)
include(data_objects/data_objects.pri)
include(data_models/data_models.pri)
include(model_views/model_views.pri)
include(data_access/data_access.pri)
include(logging/logging.pri)
include(business_objects/business_objects.pri)
include(controls/controls.pri)
include(images/images.pri)

SOURCES += \
    qt_globals.cpp

HEADERS += \
    qt_globals.h
