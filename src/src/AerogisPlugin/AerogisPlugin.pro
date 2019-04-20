TEMPLATE = lib
TARGET = AerogisPlugin
QT += qml xml quick widgets opengl serialport
CONFIG += qt plugin c++11
TRANSLATIONS = aerogis_en.ts aerogis_es.ts aerogis_ru.ts

DESTDIR = ../../bin/Aerogis
TARGET = $$qtLibraryTarget($$TARGET)

INCLUDEPATH += $(OSG_ROOT)/include \
    $(OSGEARTH_ROOT)/include \
    $(OSG_DIR)/include \
    $(OSGEARTH_DIR)/include \
    ../EarthWidget \
    ./
CONFIG(debug,debug|release) { INCLUDEPATH += debug } else { INCLUDEPATH += release }


# Input
SOURCES += \
    aerogis_plugin.cpp \
    ../EarthWidget/OSGGroup.cpp \
    ../EarthWidget/OSGNode.cpp \
    ../EarthWidget/OSGNodeFile.cpp \
    ../EarthWidget/OSGViewport.cpp \
    ../EarthWidget/QuickWindowViewer.cpp \
    ../EarthWidget/EarthWidget.cpp \
    ../EarthWidget/ShaderFix.cpp \
    ../EarthWidget/ShaderGeneratorFix.cpp \
    AerogisConfig.cpp \
    AerogisCore.cpp \
    ../EarthWidget/Utility.cpp \
	VSSProtocol.cpp \
	VSSRoutines.cpp \
	VSSWebReader.cpp

HEADERS += \
    aerogis_plugin.h \
    ../EarthWidget/Export.hpp \
    ../EarthWidget/OSGGroup.hpp \
    ../EarthWidget/OSGNode.hpp \
    ../EarthWidget/OSGNodeFile.hpp \
    ../EarthWidget/OSGViewport.hpp \
    ../EarthWidget/QuickWindowViewer.hpp \
    ../EarthWidget/Utility.hpp \
    ../EarthWidget/EarthWidget.h \
    AerogisConfig.h \
    AerogisCore.h \
    ../EarthWidget/ShaderFix.h ShaderGeneratorFix.h \
    VSSProtocol.h \
    VSSRoutines.h \
    VSSWebReader.h \
    bit.h

LIBS += -L"$(OSGEARTH_ROOT)/lib" \
    -L"$(OSG_ROOT)/lib" \
    -L"$(OSGEARTH_ROOT)/lib64" \
    -L"$(OSG_ROOT)/lib64" \
    -L"/usr/local/lib64"

CONFIG(debug,debug|release) {
LIBS += \ #-losgEarthQtd \
    -losgEarthUtild \
    -losgEarthAnnotationd \
    -losgEarthSymbologyd \
    -losgEarthFeaturesd \
    #-losgQtd \
    -losgUtild \
    -losgViewerd \
    -losgGAd \
    -losgDBd \
    -losgTextd \
    -losgd \
    -losgEarthd \
    -lOpenThreadsd
} else {
LIBS += -losgEarthQt \
    -losgEarthUtil \
    -losgEarthAnnotation \
    -losgEarthSymbology \
    -losgEarthFeatures \
    -losgQt \
    -losgUtil \
    -losgViewer \
    -losgGA \
    -losgDB \
    -losgText \
    -losg \
    -losgEarth \
    -lOpenThreads \
    -lQt5Core  -lQt5Gui -lQt5Widgets -lQt5Sensors -lQt5Quick -lQt5Qml -lQt5Xml -lQt5WebSockets
}

OTHER_FILES = qmldir \
    aero.conf \
    ../../bin/aero.earth \
    ../../bin/aero.conf \
    ../../bin/*

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

pluginfiles.files += \
    qmldir \
    AerogisWidget.qml \
    ../../bin \
    ../../bin/Aerogis/*

pluginfiles.path += \
    ./

configfiles.files += \
    ../../bin/Aerogis/*

configfiles.path += \
    ./Aerogis/

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir pluginfiles configfiles
}

RESOURCES +=



INSTALLS += target pluginfiles
DISTFILES += \
    AerogisWidget.qml
