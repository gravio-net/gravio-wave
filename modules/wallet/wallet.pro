TEMPLATE = lib
TARGET = wallet
QT += qml quick quickcontrols2
CONFIG += plugin c++11

INCLUDEPATH += ../../ ../../openssl/include

android:  LIBS += -L../gravio-wave/openssl/bin/ -lssl -lcrypto
else: LIBS += ../openssl/bin/mingw/libcrypto-1_1.a ../openssl/bin/mingw/libssl-1_1.a

# Input
SOURCES += \
    wallet_plugin.cpp \
    ../../json.cpp \
    wallet.cpp \
    wallets.cpp \
    ../../wallet/walletstore.cpp \
    ../../wallet/crypto/ctaes/ctaes.c \
    ../../wallet/crypto/sha256.cpp \
    ../../wallet/crypto/sha512.cpp \
    ../../wallet/crypto/aes.cpp \
    ../../wallet/secp256k1/src/secp256k1.c \
    ../../wallet/key.cpp \
    ../../wallet/context.cpp \
    ../../wallet/cryptoaddress.cpp \
    ../../wallet/crypto/ripemd160.cpp \
    ../../wallet/hash.cpp \
    ../../wallet/sync.cpp \
    ../../wallet/support/cleanse.cpp \
    ../../wallet/transactionstore.cpp \
    ../../wallet/script/script.cpp \
    ../../wallet/utilstrencodings.cpp \
    ../../wallet/primitives/transaction.cpp \
    ../../wallet/amount.cpp \
    ../../wallet/uint256.cpp \
    transaction.cpp \
    transactions.cpp \
    units.cpp

HEADERS += \
    wallet_plugin.h \
    ../../json.h \
    wallet.h \
    wallets.h \
    ../../iaccount.h \
    transaction.h \
    ../../wallet/uint256.h \
    ../../wallet/walletstore.h \
    ../../wallet/compat/byteswap.h \
    ../../wallet/compat/endian.h \
    ../../wallet/crypto/ctaes/ctaes.h \
    ../../wallet/crypto/aes.h \
    ../../wallet/crypto/common.h \
    ../../wallet/crypto/sha256.h \
    ../../wallet/crypto/sha512.h \
    ../../wallet/key.h \
    ../../wallet/context.h \
    ../../wallet/cryptoaddress.h \
    ../../wallet/hash.h \
    ../../wallet/crypto/ripemd160.h \
    ../../wallet/sync.h \
    ../../wallet/datastream.h \
    ../../wallet/support/cleanse.h \
    ../../wallet/transactionstore.h \
    ../../wallet/script/script.h \
    ../../wallet/utilstrencodings.h \
    ../../wallet/primitives/transaction.h \
    ../../wallet/amount.h \
    ../../wallet/version.h \
    transactions.h \
    units.h

DISTFILES += \
    wallet_ui.qml \
    wallet_instance.qml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        ../gravio-wave/openssl/bin/libcrypto.so \
        ../gravio-wave/openssl/bin/libssl.so
}
