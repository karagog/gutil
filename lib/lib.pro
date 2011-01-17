TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib

win32 {
    COPY_LIBS = *.dll
    COPY_CMD = copy
    LIB_DESTINATION = ..\\..\\lib
}
unix {
    COPY_LIBS = *.so*
    COPY_CMD = cp --no-dereference
    LIB_DESTINATION = ../../lib
}

migrate_staticlibs.commands = $$COPY_CMD *.a $$LIB_DESTINATION
migrate_sharedlibs.commands = $$COPY_CMD $$COPY_LIBS $$LIB_DESTINATION

PRE_TARGETDEPS = \
    migrate_staticlibs \
    migrate_sharedlibs

QMAKE_EXTRA_TARGETS = \
    migrate_staticlibs \
    migrate_sharedlibs

