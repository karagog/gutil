
TEMPLATE = subdirs

# Each library may depend on the ones listed above it, but necessarily don't
#   depend on libraries below it.
SUBDIRS += \
    pre_build \
    third_party \
    core \
    qt \
    qt_plugins \
    games \
    test \
    doc

CONFIG += ordered

HEADERS += \
    gutil.h

