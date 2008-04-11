TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

CAS_CPPFLAGS = $(CASINC)

CAS_LDPATH = -L$(CASLIB) -lTKV3d

INCLUDEPATH += ../../include $${CAS_CPPFLAGS}
LIBS += $${CAS_LDPATH}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

LIB_64 {
	DEFINES += _OCC64
}

HEADERS  = *.hxx
HEADERS += *.h
HEADERS += *.ixx
HEADERS += *.jxx

SOURCES  = *.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
