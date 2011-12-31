
TEMPLATE = subdirs

# Each library may depend on the ones listed above it, but necessarily don't
#   depend on libraries below it.
SUBDIRS += \
    ThirdParty \
    Core \
    Qt \
    CryptoPP \
    Test

CONFIG += ordered

HEADERS += \
    gassert.h \
    gutil_core.h \
    gutil.h \
    gutil_macros.h








