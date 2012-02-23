
# The deploy project is designed to prepare the GUtil project
#  for deployment in a software development environment.

# It has several tasks:

#  1.) Generate top-level GUtil headers (gutil_core.h, gutil_qt.h, etc...)
#  2.) Generate doxygen documentation
#  3.) TBD...

TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib


HEADER_CMD = python ../PythonUtils/GenerateHeaders.py

# The working dir for the python script
WORKING_DIR = ..

# Directory patterns for which we want to ignore all headers
IGNORE_PATHS = Test

CORE_HEADER = gutil_core.h
CORE_DIR = Core

QT_HEADER = gutil_qt.h
QT_DIR = Qt

headers_core.commands = $$HEADER_CMD --outfile $$CORE_HEADER --dirs=$$CORE_DIR --ignore-path $$IGNORE_PATHS --working-dir=$$WORKING_DIR
headers_qt.commands   = $$HEADER_CMD --outfile $$QT_HEADER --dirs=$$QT_DIR --ignore-path $$IGNORE_PATHS --working-dir=$$WORKING_DIR

PRE_TARGETDEPS = \
    headers_core \
    headers_qt

QMAKE_EXTRA_TARGETS = \
    headers_core \
    headers_qt
