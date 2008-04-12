TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

CAS_CPPFLAGS = $(CASINC)

CAS_KERNEL = -L$(CASLIB) -lTKernel

CAS_VIEWER = -L$(CASLIB) -lTKV3d -lTKService

INCLUDEPATH += ../../include  $${CAS_CPPFLAGS}
LIBS += -L../../$(CONFIG_ID)/lib $${CAS_KERNEL} $${CAS_VIEWER}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += HTMLSERVICE_EXPORTS $(CASDEFINES) OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = *.hxx

SOURCES  = *.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
