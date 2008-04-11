TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

CAS_CPPFLAGS = $(CASINC)

CAS_KERNEL = -L$(CASLIB) -lTKernel

CAS_VIEWER = -L$(CASLIB) -lTKV3d -lTKService

INCLUDEPATH += ../../include $${CAS_CPPFLAGS}
LIBS += -L../../$(CONFIG_ID)/lib $${CAS_KERNEL} $${CAS_VIEWER} -lQtx -lSUIT -lSTD

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += CAF_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

LIB_64 {
	DEFINES += _OCC64
}

HEADERS  = *.h

SOURCES  = *.cxx

TRANSLATIONS = resources/CAF_images.ts \
               resources/CAF_msg_en.ts

unix:GUIResources = ../../resources
win32:GUIResources = ..\\..\\resources

lrelease.name = LRELASE ${QMAKE_FILE_IN}
unix:lrelease.commands = $(QTDIR)/bin/lrelease ${QMAKE_FILE_NAME} -qm $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.commands = $(QTDIR)\\bin\\lrelease ${QMAKE_FILE_NAME} -qm $${GUIResources}\\${QMAKE_FILE_BASE}.qm
unix:lrelease.output = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.output = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.input = TRANSLATIONS
unix:lrelease.clean = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.clean = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.xml resources/*.ini
resources.path = ../../resources

INSTALLS += includes resources
