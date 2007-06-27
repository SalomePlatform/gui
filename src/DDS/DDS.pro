TEMPLATE = lib
TARGET = DDS
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

CAS_OCAF = -L$${CASROOT}/Linux/lib -lPTKernel -lTKernel -lTKCDF -lTKLCAF -lTKPCAF -lTKStdSchema

INCLUDEPATH += ../../include $${CAS_CPPFLAGS}
LIBS += $${CAS_KERNEL} $${CAS_OCAF}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=2 OCC_VERSION_MAINTENANCE=0 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = DDS.h
HEADERS += DDS_DicGroup.h
HEADERS += DDS_DicItem.h
HEADERS += DDS_Dictionary.h
HEADERS += DDS_KeyWords.h

SOURCES  = DDS_DicGroup.cxx
SOURCES += DDS_DicItem.cxx
SOURCES += DDS_Dictionary.cxx
SOURCES += DDS_KeyWords.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
