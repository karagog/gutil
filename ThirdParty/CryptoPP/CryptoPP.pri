
QMAKE_CXXFLAGS += -mthreads -pipe
DEFINES += NDEBUG
DEFINES -= UNICODE

SOURCES += \
    CryptoPP/algebra.cpp \
    CryptoPP/algparam.cpp \
    CryptoPP/asn.cpp \
    CryptoPP/base64.cpp \
    CryptoPP/basecode.cpp \
    CryptoPP/cbcmac.cpp \
    CryptoPP/channels.cpp \
    CryptoPP/crc.cpp \
    CryptoPP/cryptlib.cpp \
    CryptoPP/cpu.cpp \
    CryptoPP/default.cpp \
    CryptoPP/des.cpp \
    CryptoPP/dessp.cpp \
    CryptoPP/dh.cpp \
    CryptoPP/dll.cpp \
    CryptoPP/dsa.cpp \
    CryptoPP/ec2n.cpp \
    CryptoPP/eccrypto.cpp \
    CryptoPP/ecp.cpp \
    CryptoPP/emsa2.cpp \
    CryptoPP/eprecomp.cpp \
    CryptoPP/files.cpp \
    CryptoPP/filters.cpp \
    CryptoPP/fips140.cpp \
    CryptoPP/fipstest.cpp \
    CryptoPP/gf2n.cpp \
    CryptoPP/gfpcrypt.cpp \
    CryptoPP/gzip.cpp \
    CryptoPP/hex.cpp \
    CryptoPP/hmac.cpp \
    CryptoPP/hrtimer.cpp \
    CryptoPP/integer.cpp \
    CryptoPP/iterhash.cpp \
    CryptoPP/misc.cpp \
    CryptoPP/modes.cpp \
    CryptoPP/mqueue.cpp \
    CryptoPP/nbtheory.cpp \
    CryptoPP/oaep.cpp \
    CryptoPP/osrng.cpp \
    CryptoPP/pch.cpp \
    CryptoPP/pkcspad.cpp \
    CryptoPP/pssr.cpp \
    CryptoPP/pubkey.cpp \
    CryptoPP/queue.cpp \
    CryptoPP/randpool.cpp \
    CryptoPP/rdtables.cpp \
    CryptoPP/rijndael.cpp \
    CryptoPP/rng.cpp \
    CryptoPP/rsa.cpp \
    CryptoPP/sha.cpp \
    CryptoPP/simple.cpp \
    CryptoPP/skipjack.cpp \
    CryptoPP/strciphr.cpp \
    CryptoPP/trdlocal.cpp \
    CryptoPP/vmac.cpp \
    CryptoPP/zdeflate.cpp \
    CryptoPP/zinflate.cpp
