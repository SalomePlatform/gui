TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

CAS_VIEWER = -L$${CASROOT}/Linux/lib -lTKV3d -lTKService

INCLUDEPATH += ../../include  $${CAS_CPPFLAGS}
LIBS += -L../../$(CONFIG_ID)/lib $${CAS_KERNEL} $${CAS_VIEWER}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += HTMLSERVICE_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

LIB_64 {
	DEFINES += _OCC64
}

HEADERS  = *.hxx

SOURCES  = *.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
