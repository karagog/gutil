TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib

#To build this library of libraries, first extract the objects from the constituent
#  libraries, then bundle them up in the new one
extract_controls.commands = ar -x libControls.a
extract_custom.commands = ar -x libCustom.a
extract_dataaccess.commands = ar -x libDataAccess.a
extract_tools.commands = ar -x libTools.a
extract_utils.commands = ar -x libUtils.a

buildlib.commands = ar -ru ../libGQtUtil.a *.o

cleanup.commands = rm *.o

PRE_TARGETDEPS += \
  extract_controls \
  extract_custom \
  extract_dataaccess \
  extract_tools \
  extract_utils \
  buildlib \
  cleanup \

QMAKE_EXTRA_TARGETS += \
  extract_controls \
  extract_custom \
  extract_dataaccess \
  extract_tools \
  extract_utils \
  buildlib \
  cleanup

