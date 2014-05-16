
# The pre-build project is designed to prepare the GUtil project
#  for building in a software development environment.

# It has several tasks:

#  1.) Generate top-level GUtil headers (gutil_core.h, gutil_qt.h, etc...)
#  2.) Generate doxygen documentation (and compress it in a .zip)
#  3.) TBD...

TEMPLATE = lib
CONFIG += staticlib

TOP_DIR = ../..

HEADER_CMD = python $$TOP_DIR/scripts/GenerateHeaders.py
HEADER_PREFIX = gutil_

# Directory patterns for which we want to ignore all headers
IGNORE_PATHS = Test

HEADERGEN_TARGET_DIRS = core,qt,cryptopp,qt_plugins,games


headers.commands = $$HEADER_CMD \
                        --working-dir=.. \
                        --output-dir=../include \
                        --input-dirs=$$HEADERGEN_TARGET_DIRS \
                        --ignore-path=$$IGNORE_PATHS \
                        --output-prefix=$$HEADER_PREFIX

PRE_TARGETDEPS =  headers

QMAKE_EXTRA_TARGETS =  headers
