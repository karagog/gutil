
!exists(CryptoPP/libcryptopp.a){
    build_cryptopp.commands = cd CryptoPP; make libcryptopp.a;
}


#TEMP_LIBDIRECTORY = ../lib

#!exists($$TEMP_LIBDIRECTORY/libcryptopp.a){
#    export_cryptopp.commands = cp CryptoPP/libcryptopp.a $$TEMP_LIBDIRECTORY;
#}

PRE_TARGETDEPS += build_cryptopp

QMAKE_EXTRA_TARGETS += build_cryptopp
