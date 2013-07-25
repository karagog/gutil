
QT -= gui core
DESTDIR = ../../lib
TARGET = cryptopp
TEMPLATE = lib

# Add this to build the static library
#CONFIG += staticlib

# Disable warnings, because we don't develop this library anyways
CONFIG += warn_off

INCLUDEPATH += ..
QMAKE_CXXFLAGS += -pipe

win32{
  QMAKE_CXXFLAGS += -mthreads
  LIBS += -lws2_32
}

DEFINES += NDEBUG
DEFINES -= UNICODE

SOURCES += \
    3way.cpp \
    adler32.cpp \
    algparam.cpp \
    arc4.cpp \
    asn.cpp \
    authenc.cpp \
    base32.cpp \
    base64.cpp \
    basecode.cpp \
    bfinit.cpp \
    blowfish.cpp \
    blumshub.cpp \
    camellia.cpp \
    cast.cpp \
    casts.cpp \
    cbcmac.cpp \
    ccm.cpp \
    channels.cpp \
    cmac.cpp \
    cpu.cpp \
    crc.cpp \
    cryptlib.cpp \
    cryptlib_bds.cpp \
    default.cpp \
    des.cpp \
    dessp.cpp \
    dh.cpp \
    dh2.cpp \
    dll.cpp \
    dsa.cpp \
    eax.cpp \
    ec2n.cpp \
    ecp.cpp \
    elgamal.cpp \
    emsa2.cpp \
    esign.cpp \
    files.cpp \
    filters.cpp \
    fips140.cpp \
    gcm.cpp \
    gf2_32.cpp \
    gf2n.cpp \
    gf256.cpp \
    gfpcrypt.cpp \
    gost.cpp \
    gzip.cpp \
    hex.cpp \
    hmac.cpp \
    hrtimer.cpp \
    ida.cpp \
    idea.cpp \
    integer.cpp \
    iterhash.cpp \
    luc.cpp \
    mars.cpp \
    marss.cpp \
    md2.cpp \
    md4.cpp \
    md5.cpp \
    misc.cpp \
    modes.cpp \
    mqueue.cpp \
    mqv.cpp \
    nbtheory.cpp \
    network.cpp \
    oaep.cpp \
    osrng.cpp \
    panama.cpp \
    pch.cpp \
    pkcspad.cpp \
    polynomi.cpp \
    pssr.cpp \
    pubkey.cpp \
    queue.cpp \
    rabin.cpp \
    randpool.cpp \
    rc2.cpp \
    rc5.cpp \
    rc6.cpp \
    rdtables.cpp \
    rijndael.cpp \
    ripemd.cpp \
    rng.cpp \
    rsa.cpp \
    rw.cpp \
    safer.cpp \
    salsa.cpp \
    seal.cpp \
    seed.cpp \
    serpent.cpp \
    sha.cpp \
    shacal2.cpp \
    shark.cpp \
    sharkbox.cpp \
    simple.cpp \
    skipjack.cpp \
    socketft.cpp \
    sosemanuk.cpp \
    square.cpp \
    squaretb.cpp \
    tea.cpp \
    tftables.cpp \
    tiger.cpp \
    tigertab.cpp \
    trdlocal.cpp \
    ttmac.cpp \
    twofish.cpp \
    vmac.cpp \
    wait.cpp \
    wake.cpp \
    whrlpool.cpp \
    winpipes.cpp \
    xtr.cpp \
    xtrcrypt.cpp \
    zdeflate.cpp \
    zinflate.cpp \
    zlib.cpp

HEADERS += \
    zlib.h \
    zinflate.h \
    zdeflate.h \
    xtrcrypt.h \
    xtr.h \
    words.h \
    winpipes.h \
    whrlpool.h \
    wake.h \
    wait.h \
    vmac.h \
    validate.h \
    twofish.h \
    ttmac.h \
    trunhash.h \
    trdlocal.h \
    tiger.h \
    tea.h \
    strciphr.h \
    stdcpp.h \
    square.h \
    sosemanuk.h \
    socketft.h \
    smartptr.h \
    skipjack.h \
    simple.h \
    shark.h \
    shacal2.h \
    sha.h \
    serpentp.h \
    serpent.h \
    seed.h \
    seckey.h \
    secblock.h \
    seal.h \
    salsa.h \
    safer.h \
    rw.h \
    rsa.h \
    rng.h \
    ripemd.h \
    rijndael.h \
    resource.h \
    rc6.h \
    rc5.h \
    rc2.h \
    randpool.h \
    rabin.h \
    queue.h \
    pwdbased.h \
    pubkey.h \
    pssr.h \
    polynomi.h \
    pkcspad.h \
    pch.h \
    panama.h \
    osrng.h \
    oids.h \
    oaep.h \
    nr.h \
    network.h \
    nbtheory.h \
    mqv.h \
    mqueue.h \
    modexppc.h \
    modes.h \
    modarith.h \
    misc.h \
    mdc.h \
    md5.h \
    md4.h \
    md2.h \
    mars.h \
    luc.h \
    lubyrack.h \
    iterhash.h \
    integer.h \
    idea.h \
    ida.h \
    hrtimer.h \
    hmac.h \
    hex.h \
    gzip.h \
    gost.h \
    gfpcrypt.h \
    gf256.h \
    gf2n.h \
    gf2_32.h \
    gcm.h \
    fltrimpl.h \
    fips140.h \
    filters.h \
    files.h \
    factory.h \
    esign.h \
    eprecomp.h \
    emsa2.h \
    elgamal.h \
    ecp.h \
    eccrypto.h \
    ec2n.h \
    eax.h \
    dsa.h \
    dmac.h \
    dll.h \
    dh2.h \
    dh.h \
    des.h \
    default.h \
    cryptlib.h \
    crc.h \
    cpu.h \
    config.h \
    cmac.h \
    channels.h \
    ccm.h \
    cbcmac.h \
    cast.h \
    camellia.h \
    blumshub.h \
    blowfish.h \
    bench.h \
    basecode.h \
    base64.h \
    base32.h \
    authenc.h \
    asn.h \
    argnames.h \
    arc4.h \
    algparam.h \
    algebra.h \
    aes.h \
    adler32.h \
    3way.h
