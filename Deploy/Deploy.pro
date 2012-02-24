
# The deploy project is designed to prepare the GUtil project
#  for deployment in a software development environment.

# It has several tasks:

#  1.) Generate top-level GUtil headers (gutil_core.h, gutil_qt.h, etc...)
#  2.) Generate doxygen documentation
#  3.) TBD...

TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib


ZIP_CMD = zip
HEADER_CMD = python ../PythonUtils/GenerateHeaders.py

DOCS_COMMAND = doxygen
DOCS_PATH = ../doc
DOCS_DIR = html
DOCS_ZIP_TARGET = GUtil_docs.zip

# The working dir for the python script
HEADERGEN_WORKING_DIR = ..

# Directory patterns for which we want to ignore all headers
IGNORE_PATHS = Test

CORE_HEADER = gutil_core.h
CORE_DIR = Core

QT_HEADER = gutil_qt.h
QT_DIR = Qt


headers_core.commands = $$HEADER_CMD --outfile $$CORE_HEADER --dirs=$$CORE_DIR --ignore-path $$IGNORE_PATHS --working-dir=$$HEADERGEN_WORKING_DIR
headers_qt.commands   = $$HEADER_CMD --outfile $$QT_HEADER --dirs=$$QT_DIR --ignore-path $$IGNORE_PATHS --working-dir=$$HEADERGEN_WORKING_DIR

exists(($$DOCS_PATH/$$DOCS_DIR)){
    docs_clean.commands = rm $$DOCS_PATH/$$DOCS_DIR -rf
} else{
    docs_clean.commands =
}
exists(($$DOCS_PATH/$$DOCS_ZIP_TARGET)){
    docs_clean_zip.commands = rm $$DOCS_PATH/$$DOCS_ZIP_TARGET
} else{
    docs_clean_zip.commands =
}

docs_gen.commands = $$DOCS_COMMAND
docs_zip.commands = cd $$DOCS_PATH; $$ZIP_CMD $$DOCS_ZIP_TARGET -r $$DOCS_DIR

PRE_TARGETDEPS = \
    headers_core \
    headers_qt \
    docs_clean \
    docs_gen \
    docs_clean_zip \
    docs_zip

QMAKE_EXTRA_TARGETS = \
    headers_core \
    headers_qt \
    docs_clean \
    docs_gen \
    docs_clean_zip \
    docs_zip
