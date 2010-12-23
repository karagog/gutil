
build_cryptopp.commands = \
    if [ `ls CryptoPP -l | grep libcryptopp.a | wc -l` = 0 ]; then \
        cd CryptoPP; make libcryptopp.a; cd ..; \
    fi;

QMAKE_EXTRA_TARGETS += build_cryptopp
