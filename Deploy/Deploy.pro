
# The deploy project is designed to prepare the GUtil project
#  for deployment in a software development environment.

# It has several tasks:

#  1.) Generate top-level GUtil headers (gutil_core.h, gutil_qt.h, etc...)
#  2.) Generate doxygen documentation
#  3.)

TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib

win32 {
    COPY_LIBS = *.dll *.a
    COPY_CMD = cp
}
unix {
    COPY_LIBS = *.so*
    COPY_CMD = cp --no-dereference
}

LIB_DESTINATION = ../../lib

migrate_libs.commands = $$COPY_CMD $$COPY_LIBS $$LIB_DESTINATION $$COPY_ARGS

#PRE_TARGETDEPS = \
#    migrate_staticlibs \
#    migrate_libs

#QMAKE_EXTRA_TARGETS = \
#    migrate_staticlibs \
#    migrate_libs
