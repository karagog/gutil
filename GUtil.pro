
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
    DataObjects \
    DataModels \
    ModelViews \
    DataAccess \
    Logging \
    BusinessObjects \
    Controls \
    Images \
    GUICustom \
    Test

CONFIG += ordered

HEADERS += \
    gassert.h \
    gutil.h \
    gutil_macros.h




