
build_cryptopp.commands = \
    if [ `ls CryptoPP -l | grep libcryptopp.a | wc -l` = 0 ]; then \
        cd CryptoPP; make libcryptopp.a; \
    fi;


#TEMP_LIBDIRECTORY = ../lib

#!exists($$TEMP_LIBDIRECTORY/libcryptopp.a){
#    export_cryptopp.commands = cp CryptoPP/libcryptopp.a $$TEMP_LIBDIRECTORY;
#}

PRE_TARGETDEPS += build_cryptopp

QMAKE_EXTRA_TARGETS += build_cryptopp
