DESTDIR = ../lib

TARGET = GUtilQt
#VERSION = 0.0.0

TEMPLATE = lib

#QT -= gui
#DEFINES += GUTIL_NO_GUI_FUNCTIONALITY

QT += sql
#DEFINES += GUTIL_NO_DATABASE_FUNCTIONALITY

QT += network
#DEFINES += GUTIL_NO_NETWORK_FUNCTIONALITY

#DEFINES += DEBUG

# So we can interface our core classes more easily with Qt
DEFINES += QT_ADAPTERS

INCLUDEPATH += ../include ../ThirdParty
LIBS += -L../lib -lGUtil

include(Interfaces/Interfaces.pri)
include(Utils/Utils.pri)
include(Custom/Custom.pri)
include(Ui/Ui.pri)
include(DataObjects/DataObjects.pri)
include(DataModels/DataModels.pri)
include(ModelViews/ModelViews.pri)
include(DataAccess/DataAccess.pri)
include(Logging/Logging.pri)
include(BusinessObjects/BusinessObjects.pri)
include(Controls/Controls.pri)
include(Images/Images.pri)
