
#TARGET = GQtUtil
#DESTDIR = ..
TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib

#gqtutil.target = GQtUtil
#gqtutil.commands = make libcryptopp.a

#PRE_TARGETDEPS += gqtutil
#QMAKE_EXTRA_TARGETS += gqtutil

copytarget.commands = ar -ru libGQtUtil.a libTools.a libCustom.a libDataAccess.a libUtils.a
PRE_TARGETDEPS += copytarget
QMAKE_EXTRA_TARGETS += copytarget

