TEMPLATE = app

QT += qml quick

SOURCES += main.cpp

RESOURCES +=

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = "/home/mugen/WorkProjects/Aerogis/src/bin/"

# Default rules for deployment.
include(deployment.pri)

HEADERS +=

DESTDIR = ../../bin

OTHER_FILES += \
    ../../bin/main.qml

