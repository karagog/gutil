
build_cryptopp.commands = \
    if [ `ls CryptoPP -l | grep libcryptopp.a | wc -l` = 0 ]; then \
        cd CryptoPP; make libcryptopp.a; cd ..; \
    fi;

PRE_TARGETDEPS = build_cryptopp

QMAKE_EXTRA_TARGETS += build_cryptopp
