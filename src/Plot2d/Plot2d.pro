TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

QWTHOME=$$(QWTHOME)
QWTINC=$${QWTHOME}/includes
QWTLIB=$${QWTHOME}/lib

INCLUDEPATH += ../../include $${QWTINC} $$(PYTHONINC)
INCLUDEPATH += ../Qtx ../SUIT
unix:LIBS  += -L$${QWTLIB} -lqwt
win32:LIBS += /LIBPATH:$$(QWTLIB)
LIBS += -L../../$(CONFIG_ID)/lib -lQtx -lSUIT

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PLOT2D_EXPORTS

HEADERS  = *.h

SOURCES  = *.cxx

TRANSLATIONS = resources/Plot2d_msg_en.ts \
               resources/Plot2d_images.ts

unix:GUIResources = ../../resources
win32:GUIResources = ..\\..\\resources

lrelease.name = LRELASE ${QMAKE_FILE_IN}
unix:lrelease.commands = $(QTDIR)/$(CONFIG_ID)/bin/lrelease ${QMAKE_FILE_NAME} -qm $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.commands = $(QTDIR)\\$(CONFIG_ID)\\bin\\lrelease ${QMAKE_FILE_NAME} -qm $${GUIResources}\\${QMAKE_FILE_BASE}.qm
unix:lrelease.output = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.output = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.input = TRANSLATIONS
unix:lrelease.clean = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.clean = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

ICONS = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS
resources.path = ../../resources

INSTALLS += includes resources
