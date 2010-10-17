TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib

#To build this library of libraries, first extract the objects from the constituent
#  libraries, then bundle them up in the new one
extract_controls.commands = ar -x libControls.a
extract_core.commands = ar -x libCore.a
extract_custom.commands = ar -x libCustom.a
extract_dataaccess.commands = ar -x libDataAccess.a
#extract_dataobjects.commands = ar -x libDataObjects.a
extract_businessobjects.commands = ar -x libBusinessObjects.a
extract_tools.commands = ar -x libTools.a
extract_utils.commands = ar -x libUtils.a


BUILD_LIBRARY = "libGUtil.a"

buildlib.commands = ar -ru $$BUILD_LIBRARY *.o
copy_lib.commands = cp $$BUILD_LIBRARY ..
cleanup.commands = rm *.o

PRE_TARGETDEPS += \
  extract_controls \
  extract_core \
  extract_custom \
  extract_dataaccess \
  #extract_dataobjects \
  extract_businessobjects \
  extract_tools \
  extract_utils \
  buildlib \
  copy_lib \
  cleanup \

QMAKE_EXTRA_TARGETS += \
  extract_controls \
  extract_core \
  extract_custom \
  extract_dataaccess \
  #extract_dataobjects \
  extract_businessobjects \
  extract_tools \
  extract_utils \
  buildlib \
  copy_lib \
  cleanup

