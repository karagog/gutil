
TEMPLATE = subdirs

# Each library may depend on the ones listed above it, but necessarily don't
#   depend on libraries below it.
SUBDIRS += \
    ThirdParty \
    Core \
    Qt \
    CryptoPP \
    Test \
    Deploy

CONFIG += ordered

HEADERS += \
    gutil.h








