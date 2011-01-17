
QT -= gui core
DESTDIR = ../../lib
TARGET = cryptopp
TEMPLATE = lib

QMAKE_CXXFLAGS += -mthreads -pipe
DEFINES += NDEBUG
DEFINES -= UNICODE

SOURCES += \
    algebra.cpp \
    algparam.cpp \
    asn.cpp \
    base64.cpp \
    basecode.cpp \
    cbcmac.cpp \
    channels.cpp \
    crc.cpp \
    cryptlib.cpp \
    cpu.cpp \
    default.cpp \
    des.cpp \
    dessp.cpp \
    dh.cpp \
    dll.cpp \
    dsa.cpp \
    ec2n.cpp \
    eccrypto.cpp \
    ecp.cpp \
    emsa2.cpp \
    eprecomp.cpp \
    files.cpp \
    filters.cpp \
    fips140.cpp \
    fipstest.cpp \
    gf2n.cpp \
    gfpcrypt.cpp \
    gzip.cpp \
    hex.cpp \
    hmac.cpp \
    hrtimer.cpp \
    integer.cpp \
    iterhash.cpp \
    misc.cpp \
    modes.cpp \
    mqueue.cpp \
    nbtheory.cpp \
    oaep.cpp \
    osrng.cpp \
    pch.cpp \
    pkcspad.cpp \
    pssr.cpp \
    pubkey.cpp \
    queue.cpp \
    randpool.cpp \
    rdtables.cpp \
    rijndael.cpp \
    rng.cpp \
    rsa.cpp \
    sha.cpp \
    simple.cpp \
    skipjack.cpp \
    strciphr.cpp \
    trdlocal.cpp \
    vmac.cpp \
    zdeflate.cpp \
    zinflate.cpp
