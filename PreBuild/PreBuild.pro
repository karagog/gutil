
# The pre-build project is designed to prepare the GUtil project
#  for building in a software development environment.

# It has several tasks:

#  1.) Generate top-level GUtil headers (gutil_core.h, gutil_qt.h, etc...)
#  2.) Generate doxygen documentation (and compress it in a .zip)
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

HEADERGEN_TARGET_DIRS = Core,Qt,QtPlugins,CryptoPP


headers.commands = $$HEADER_CMD \
                        --working-dir=$$HEADERGEN_WORKING_DIR \
                        --input-dirs=$$HEADERGEN_TARGET_DIRS \
                        --ignore-path=$$IGNORE_PATHS \
                        --output-prefix=$$HEADER_PREFIX

docs_clean.commands =
exists($$DOCS_PATH/$$DOCS_DIR){
    docs_clean.commands = rm $$DOCS_PATH/$$DOCS_DIR -rf;
}
exists($$DOCS_PATH/$$DOCS_ZIP_TARGET){
    docs_clean.commands += rm $$DOCS_PATH/$$DOCS_ZIP_TARGET;
}

docs_gen.commands = $$DOCS_COMMAND
docs_zip.commands = cd $$DOCS_PATH; $$ZIP_CMD $$DOCS_ZIP_TARGET -r $$DOCS_DIR

PRE_TARGETDEPS =  headers
PRE_TARGETDEPS += docs_clean
PRE_TARGETDEPS += docs_gen
PRE_TARGETDEPS += docs_zip

QMAKE_EXTRA_TARGETS =  headers
QMAKE_EXTRA_TARGETS += docs_clean
QMAKE_EXTRA_TARGETS += docs_gen
QMAKE_EXTRA_TARGETS += docs_zip

OTHER_FILES += \
    Doxyfile
