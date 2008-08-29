unix:TEMPLATE = lib
win32:TEMPLATE = vclib

win32:QMAKE_MOC=$(QTDIR)\bin\moc.exe

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

CAS_CPPFLAGS = $(CASINC)

unix:CAS_LDPATH = -L$(CASLIB) -lTKV3d
win32:CAS_LDPATH = -L$(CASLIB) -lTKernel -lTKMath -lTKV3d

INCLUDEPATH += ../../include $${CAS_CPPFLAGS}
LIBS += $${CAS_LDPATH}
win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WNT WIN32 
DEFINES += $(CASDEFINES) OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = *.hxx
HEADERS += *.h
HEADERS += *.ixx
HEADERS += *.jxx

SOURCES  = *.cxx

win32:COPY_FILES = *.hxx *.h
win32:copy_hxx.name = Install(.hxx) ${QMAKE_FILE_IN}
win32:copy_hxx.commands = type ${QMAKE_FILE_IN} > ../../include/${QMAKE_FILE_BASE}.hxx
win32:copy_hxx.output = ../../include/${QMAKE_FILE_BASE}.hxx
win32:copy_hxx.input = COPY_FILES
win32:QMAKE_EXTRA_COMPILERS += copy_hxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
