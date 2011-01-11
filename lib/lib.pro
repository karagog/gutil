TEMPLATE = lib
CONFIG += staticlib

TARGET = dummy_ignorethislib


migrate_libs.commands = \
    if [ `ls -l | grep cryptopp.dll | wc -l` = 0 ]; then \
        cp ../ThirdParty/CryptoPP/cryptopp.dll .; \
    fi;

PRE_TARGETDEPS += migrate_libs
QMAKE_EXTRA_TARGETS += migrate_libs
