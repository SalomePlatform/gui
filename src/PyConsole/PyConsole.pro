TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

INCLUDEPATH += ../../include $$(PYTHONINC)
unix:LIBS  += -L../../$(CONFIG_ID)/lib -L$$(PYTHONLIB) -lpython2.5 -lSUIT -lPyInterp
win32:LIBS += /LIBPATH:$$(PYTHONLIB) ../../lib/pyinterp.lib ../../lib/suit.lib

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PYCONSOLE_EXPORTS

HEADERS = *.h

SOURCES = *.cxx

TRANSLATIONS = resources/PyConsole_msg_en.ts

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

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
