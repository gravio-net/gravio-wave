android: QT += qml quick quickcontrols2 androidextras
else: QT += qml quick quickcontrols2

CONFIG += plugin c++11

INCLUDEPATH += openssl/include

android:  LIBS += -L../gravio-wave/openssl/bin/ -lssl -lcrypto
else: LIBS += ../gravio-wave/openssl/bin/mingw/libcrypto-1_1.a ../gravio-wave/openssl/bin/mingw/libssl-1_1.a

SOURCES += main.cpp \
    logger.cpp \
    application.cpp \
    json.cpp \
    module.cpp \
    account.cpp \
    accountdb.cpp \
    wallet/walletstore.cpp \
    wallet/crypto/ctaes/ctaes.c \
    wallet/crypto/sha256.cpp \
    wallet/crypto/sha512.cpp \
    wallet/crypto/aes.cpp \
    wallet/secp256k1/src/secp256k1.c \
    wallet/key.cpp \
    wallet/context.cpp \
    wallet/cryptoaddress.cpp \
    wallet/crypto/ripemd160.cpp \
    wallet/hash.cpp \
    wallet/sync.cpp \
    wallet/support/cleanse.cpp \
    wallet/transactionstore.cpp \
    wallet/script/script.cpp \
    wallet/utilstrencodings.cpp \
    wallet/primitives/transaction.cpp \
    wallet/amount.cpp \
    wallet/uint256.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    qml/gravio-app.qml \
    gravio-app.config \
    qtquickcontrols2.conf \
    modules/modules.config \
    android/AndroidManifest.xml \
    qml/account.qml \
    qml/qqr.js \
    qml/QRCode.qml

HEADERS += \
    logger.h \
    application.h \
    module.h \
    json.h \
    exception.h \
    currency.h \
    account.h \
    accountdb.h \
    wallet/uint256.h \
    wallet/walletstore.h \
    wallet/compat/byteswap.h \
    wallet/compat/endian.h \
    wallet/crypto/ctaes/ctaes.h \
    wallet/crypto/aes.h \
    wallet/crypto/common.h \
    wallet/crypto/sha256.h \
    wallet/crypto/sha512.h \
    wallet/key.h \
    wallet/context.h \
    wallet/cryptoaddress.h \
    wallet/hash.h \
    wallet/crypto/ripemd160.h \
    wallet/sync.h \
    wallet/datastream.h \
    wallet/support/cleanse.h \
    wallet/transactionstore.h \
    wallet/script/script.h \
    wallet/utilstrencodings.h \
    wallet/primitives/transaction.h \
    wallet/amount.h \
    wallet/version.h \
    iaccount.h \
    imoduleplugin.h \
    iapplication.h

RESOURCES += \
    qtquickcontrols2.conf

dep_root.files += android/*
dep_root.path = /assets
INSTALLS += dep_root

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        ../gravio-wave/openssl/bin/libcrypto.so \
        ../gravio-wave/openssl/bin/libssl.so
}
