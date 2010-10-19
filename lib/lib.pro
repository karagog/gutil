TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib

#To build this library of libraries, first extract the objects from the constituent
#  libraries, then bundle them up in the new one
extract_controls.commands = ar -x libControls.a
extract_core.commands = ar -x libCore.a
extract_custom.commands = ar -x libCustom.a
extract_dataaccess.commands = ar -x libDataAccess.a
extract_logging.commands = ar -x libLogging.a
#extract_dataobjects.commands = ar -x libDataObjects.a
extract_businessobjects.commands = ar -x libBusinessObjects.a
extract_thirdparty.commands = ar -x libThirdParty.a
extract_utils.commands = ar -x libUtils.a


win32{
	COPY = copy
	REMOVE = del
}
unix{
	COPY = cp
	REMOVE = rm
}

BUILD_LIBRARY = libGUtil.a

buildlib.commands = ar -rus $$BUILD_LIBRARY *.o
copy_lib.commands = $$COPY $$BUILD_LIBRARY ..
cleanup.commands = $$REMOVE *.o

PRE_TARGETDEPS += \
  extract_thirdparty \
  extract_controls \
  extract_core \
  extract_custom \
  extract_dataaccess \
  extract_logging \
  #extract_dataobjects \
  extract_businessobjects \
  extract_utils \
  buildlib \
  copy_lib \
  cleanup \

QMAKE_EXTRA_TARGETS += \
  extract_thirdparty \
  extract_controls \
  extract_core \
  extract_custom \
  extract_dataaccess \
  extract_logging \
  #extract_dataobjects \
  extract_businessobjects \
  extract_utils \
  buildlib \
  copy_lib \
  cleanup

