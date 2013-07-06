
TEMPLATE = subdirs

# Each library may depend on the ones listed above it, but necessarily don't
#   depend on libraries below it.
SUBDIRS += \
    PreBuild \
    ThirdParty \
    Core \
    Qt \
    QtPlugins \
    Games \
    Test

CONFIG += ordered

HEADERS += \
    gutil.h

OTHER_FILES += \
    PythonUtils/GenerateHeaders.py \
    PythonUtils/MakeSymlinksToLibs.py
