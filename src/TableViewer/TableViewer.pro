unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

win32:QMAKE_MOC=$(QTDIR)\bin\moc.exe

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

CAS_CPPFLAGS = $(CASINC)
CAS_KERNEL = -L$(CASLIB) -lTKernel

INCLUDEPATH = ../../include  $${CAS_CPPFLAGS}
LIBS += -L../../$(CONFIG_ID)/lib -lQtx -lSUIT $${CAS_KERNEL} -lHTMLService
win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32
DEFINES += TABLEVIEWER_EXPORTS $(CASDEFINES) OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS = *.h

SOURCES = *.cxx

TRANSLATIONS = resources/TableViewer_images.ts \
               resources/TableViewer_msg_en.ts

unix:GUIResources = ../../resources
win32:GUIResources = ..\\..\\resources

lrelease.name = LRELASE ${QMAKE_FILE_IN}
unix:lrelease.commands = $(QTDIR)/bin/lrelease ${QMAKE_FILE_NAME} -qm $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.commands = $(QTDIR)\\bin\\lrelease ${QMAKE_FILE_IN} -qm $${GUIResources}\\${QMAKE_FILE_BASE}.qm
unix:lrelease.output = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.output = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.input = TRANSLATIONS
unix:lrelease.clean = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.clean = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

win32:copy_hdr.name = Install ${QMAKE_FILE_IN}
win32:copy_hdr.commands = type ${QMAKE_FILE_IN} > ../../include/${QMAKE_FILE_BASE}.h
win32:copy_hdr.output = ../../include/${QMAKE_FILE_BASE}.h
win32:copy_hdr.input = HEADERS
win32:QMAKE_EXTRA_COMPILERS += copy_hdr

ICONS   = resources/*.png

win32:Resource=$$ICONS
win32:SOURCES+=$$ICONS
win32:copy_res.name = Install resources ${QMAKE_FILE_IN}
win32:copy_res.commands = type ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
win32:copy_res.output = $${GUIResources}\\${QMAKE_FILE_BASE}.png
win32:copy_res.input = Resource
win32:QMAKE_EXTRA_COMPILERS += copy_res

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS
resources.path = ../../resources

INSTALLS += includes resources
