TEMPLATE = lib
TARGET = osgdb_osgearth_label_annotation_fix
CONFIG += c++11

DESTDIR = ../../bin/osgPlugins-3.4.0
TARGET = $$qtLibraryTarget($$TARGET)

INCLUDEPATH += $(OSG_ROOT)/include \
    $(OSGEARTH_ROOT)/include \
    $(OSG_DIR)/include \
    $(OSGEARTH_DIR)/include \
    ../EarthWidget \
    ./
CONFIG(debug,debug|release) { INCLUDEPATH += debug } else { INCLUDEPATH += release }

SOURCES += \
	AnnotationLabelSourceFix.cpp \
	PlaceNodeFix.cpp

HEADERS += \
	AnnotationLabelSourceFix.h \
	PlaceNodeFix.h

LIBS += -L"$(OSGEARTH_ROOT)/lib" \
    -L"$(OSG_ROOT)/lib" \
    -L"$(OSGEARTH_ROOT)/lib64" \
    -L"$(OSG_ROOT)/lib64" \
    -L"/usr/local/lib64"

CONFIG(debug,debug|release) {
LIBS += \
    -losgEarthUtild \
    -losgEarthAnnotationd \
    -losgEarthSymbologyd \
    -losgEarthFeaturesd \
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
    -losgUtil \
    -losgViewer \
    -losgGA \
    -losgDB \
    -losgText \
    -losg \
    -losgEarth \
    -lOpenThreads
}

INSTALLS += target

