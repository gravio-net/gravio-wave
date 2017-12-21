TEMPLATE = lib
TARGET = contacts
QT += qml quick quickcontrols2
CONFIG += plugin c++11

#TARGET = $$qtLibraryTarget($$TARGET)
#uri = net.gravio.wave.modules.contacts

INCLUDEPATH += ../../ ../../openssl/include

# Input
SOURCES += \
    contacts_plugin.cpp \
    contacts.cpp \
    contact.cpp \
    contactsdb.cpp \
    ../../json.cpp

HEADERS += \
    contacts_plugin.h \
    contacts.h \
    contact.h \
    contactsdb.h \
    ../../json.h

#DISTFILES = qmldir \
#    contacts_ui.qml

#!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
#    copy_qmldir.target = $$OUT_PWD/qmldir
#    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
#    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
#    QMAKE_EXTRA_TARGETS += copy_qmldir
#    PRE_TARGETDEPS += $$copy_qmldir.target
#}

#qmldir.files = qmldir
#unix {
#    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
#    qmldir.path = $$installPath
#    target.path = $$installPath
#    INSTALLS += target qmldir
#}

DISTFILES += \
    contacts_ui.qml \
    filedialog.qml \
    cameraview.qml
