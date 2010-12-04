
TEMPLATE = subdirs

# Each library may depend on the ones listed above it, but necessarily don't
#   depend on libraries below it.
SUBDIRS += \
    PreBuild \
    ThirdParty \
    Core \
    Interfaces \
    Utils \
    Custom \
    DataObjects \
    DataModels \
    DataAccess \
    Logging \
    BusinessObjects \
    Controls \
    lib

CONFIG += ordered

HEADERS += gutil_macros.h
