
TEMPLATE = subdirs

# Each library may depend on the ones listed above it, but necessarily don't
#   depend on libraries below it.
SUBDIRS += \
    ThirdParty \
    Core \
    Interfaces \
    Utils \
    Custom \
    GUICustom \
    DataObjects \
    DataModels \
    ModelViews \
    DataAccess \
    Logging \
    BusinessObjects \
    Controls \
    Test 

CONFIG += ordered

HEADERS += gutil_macros.h
