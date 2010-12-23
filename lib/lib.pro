TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib

#To build this library of libraries, first extract the objects from the constituent
#  libraries, then bundle them up in the new one
extract_controls.commands = ar -x libGUtilControls.a
extract_core.commands = ar -x libGUtilCore.a
extract_interfaces.commands = ar -x libGUtilInterfaces.a
extract_custom.commands = ar -x libGUtilCustom.a
extract_dataobjects.commands = ar -x libGUtilDataObjects.a
extract_datamodels.commands = ar -x libGUtilDataModels.a
extract_dataaccess.commands = ar -x libGUtilDataAccess.a
extract_logging.commands = ar -x libGUtilLogging.a
extract_businessobjects.commands = ar -x libGUtilBusinessObjects.a
extract_thirdparty.commands = ar -x libGUtilThirdParty.a
extract_utils.commands = ar -x libGUtilUtils.a


BUILD_LIBRARY = libGUtil.a
win32{
    BUILD_SHARED_LIBRARY = GUtil.dll
}
unix{
    BUILD_SHARED_LIBRARY = libGUtil.so
}

buildlib.commands = ar -rus $$BUILD_LIBRARY *.o
copy_lib.commands = cp $$BUILD_LIBRARY $$BUILD_SHARED_LIBRARY ..
cleanup.commands = rm *.o

#PRE_TARGETDEPS += \
#  extract_thirdparty \
#  extract_core \
#  extract_interfaces \
#  extract_utils \
#  extract_custom \
#  extract_dataobjects \
#  extract_datamodels \
#  extract_dataaccess \
#  extract_logging \
#  extract_businessobjects \
#  extract_controls \
#  buildlib \
#  copy_lib \
#  cleanup \

#QMAKE_EXTRA_TARGETS += \
#  extract_thirdparty \
#  extract_core \
#  extract_interfaces \
#  extract_utils \
#  extract_custom \
#  extract_dataobjects \
#  extract_datamodels \
#  extract_dataaccess \
#  extract_logging \
#  extract_businessobjects \
#  extract_controls \
#  buildlib \
#  copy_lib \
#  cleanup

