
build_cryptopp.commands = \
    if [ `ls CryptoPP -l | grep libcryptopp.dll.a | wc -l` = 0 ]; then \
        cd CryptoPP; make cryptopp.dll; cd ..; \
    fi;

PRE_TARGETDEPS = build_cryptopp

QMAKE_EXTRA_TARGETS += build_cryptopp
