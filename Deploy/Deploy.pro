
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
HEADER_PREFIX = gutil_

DOCS_COMMAND = doxygen
DOCS_PATH = ../doc
DOCS_DIR = html
DOCS_ZIP_TARGET = GUtil_docs.zip

# The working dir for the python script
HEADERGEN_WORKING_DIR = ..

# Directory patterns for which we want to ignore all headers
IGNORE_PATHS = Test

HEADERGEN_TARGET_DIRS = Core,Qt,CryptoPP


headers.commands = $$HEADER_CMD \
                        --working-dir=$$HEADERGEN_WORKING_DIR \
                        --input-dirs=$$HEADERGEN_TARGET_DIRS \
                        --ignore-path=$$IGNORE_PATHS \
                        --output-prefix=$$HEADER_PREFIX

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
    headers \
    docs_clean \
    docs_gen \
    docs_clean_zip \
    docs_zip

QMAKE_EXTRA_TARGETS = \
    headers \
    docs_clean \
    docs_gen \
    docs_clean_zip \
    docs_zip
