
TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib


cleanup_libs.commands = clean_libs.bash
PRE_TARGETDEPS += cleanup_libs
QMAKE_EXTRA_TARGETS += cleanup_libs
