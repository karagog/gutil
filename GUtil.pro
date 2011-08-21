
TEMPLATE = subdirs

# Each library may depend on the ones listed above it, but necessarily don't
#   depend on libraries below it.
SUBDIRS += \
    ThirdParty \
    Core \
    Interfaces \
    Utils \
    Custom \
    Ui \
    GUICustom \
    DataObjects \
    DataModels \
    ModelViews \
    DataAccess \
    Logging \
    BusinessObjects \
    Controls \
    Images \
    Test

CONFIG += ordered

HEADERS += \
    gutil_macros.h \
    gutil_globals.h
