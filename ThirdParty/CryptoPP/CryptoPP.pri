
build_cryptopp.commands = \
    if [ `ls CryptoPP -l | grep libcryptopp.a | wc -l` = 0 ]; then \
        cd CryptoPP; \
        make libcryptopp.a; \
        cd ..; \
    fi;

LIBDIRECTORY = ../lib

export_cryptopp.commands = \
    if [ `ls $$LIBDIRECTORY -l | grep libcryptopp.a | wc -l` = 0 ]; then \
        cp CryptoPP/libcryptopp.a $$LIBDIRECTORY; \
    fi;

PRE_TARGETDEPS += build_cryptopp export_cryptopp

QMAKE_EXTRA_TARGETS += build_cryptopp export_cryptopp
