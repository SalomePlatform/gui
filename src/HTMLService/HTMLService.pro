unix:TEMPLATE = lib
win32:TEMPLATE = vclib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

CAS_CPPFLAGS = $(CASINC)

CAS_KERNEL = -L$(CASLIB) -lTKernel

CAS_VIEWER = -L$(CASLIB) -lTKV3d -lTKService

INCLUDEPATH += ../../include  $${CAS_CPPFLAGS}
LIBS += -L../../$(CONFIG_ID)/lib $${CAS_KERNEL} $${CAS_VIEWER}
win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:copy_hdr.name = Install ${QMAKE_FILE_IN}
win32:copy_hdr.commands = type ${QMAKE_FILE_IN} > ../../include/${QMAKE_FILE_BASE}.hxx
win32:copy_hdr.output = ../../include/${QMAKE_FILE_BASE}.hxx
win32:copy_hdr.input = HEADERS
win32:QMAKE_EXTRA_COMPILERS += copy_hdr

win32:DEFINES += WNT WIN32 
DEFINES += HTMLSERVICE_EXPORTS $(CASDEFINES) OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = *.hxx

SOURCES  = *.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
