TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib

#To build this library of libraries, first extract the objects from the constituent
#  libraries, then bundle them up in the new one
extract_controls.commands = ar -x libControls.a
extract_core.commands = ar -x libCore.a
extract_interfaces.commands = ar -x libInterfaces.a
extract_custom.commands = ar -x libCustom.a
extract_dataobjects.commands = ar -x libDataObjects.a
extract_datamodels.commands = ar -x libDataModels.a
extract_dataaccess.commands = ar -x libDataAccess.a
extract_logging.commands = ar -x libLogging.a
extract_businessobjects.commands = ar -x libBusinessObjects.a
extract_thirdparty.commands = ar -x libThirdParty.a
extract_utils.commands = ar -x libUtils.a


BUILD_LIBRARY = libGUtil.a

buildlib.commands = ar -rus $$BUILD_LIBRARY *.o
copy_lib.commands = cp $$BUILD_LIBRARY ..
cleanup.commands = rm *.o

PRE_TARGETDEPS += \
  extract_thirdparty \
  extract_core \
  extract_interfaces \
  extract_utils \
  extract_custom \
  extract_dataobjects \
  extract_datamodels \
  extract_dataaccess \
  extract_logging \
  extract_businessobjects \
  extract_controls \
  buildlib \
  copy_lib \
  cleanup \

QMAKE_EXTRA_TARGETS += \
  extract_thirdparty \
  extract_core \
  extract_interfaces \
  extract_utils \
  extract_custom \
  extract_dataobjects \
  extract_datamodels \
  extract_dataaccess \
  extract_logging \
  extract_businessobjects \
  extract_controls \
  buildlib \
  copy_lib \
  cleanup

