unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

win32:QMAKE_MOC=$(QTDIR)\bin\moc.exe

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

INCLUDEPATH += ../../include $$(QWTINC) $$(PYTHONINC)
INCLUDEPATH += ../Qtx ../SUIT
unix:LIBS  += -L$$(QWTLIB) -lqwt
win32:LIBS += /LIBPATH:$$(QWTLIB) -lqwt5
LIBS += -L../../$(CONFIG_ID)/lib -lQtx -lSUIT
win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32 QWT_DLL
DEFINES += PLOT2D_EXPORTS

HEADERS  = *.h

SOURCES  = *.cxx

TRANSLATIONS = resources/Plot2d_msg_en.ts \
               resources/Plot2d_images.ts

unix:GUIResources = ../../resources
win32:GUIResources = ..\\..\\resources

win32:copy_hdr.name = Install headers ${QMAKE_FILE_IN}
win32:copy_hdr.commands = type ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
win32:copy_hdr.output = ../../include/${QMAKE_FILE_BASE}.h
win32:copy_hdr.input = HEADERS
win32:QMAKE_EXTRA_COMPILERS += copy_hdr

lrelease.name = LRELASE ${QMAKE_FILE_IN}
unix:lrelease.commands = $(QTDIR)/bin/lrelease ${QMAKE_FILE_NAME} -qm ${QMAKE_FILE_OUT}
win32:lrelease.commands = $(QTDIR)\\bin\\lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
unix:lrelease.output = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.output = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.input = TRANSLATIONS
unix:lrelease.clean = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.clean = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

ICONS = resources/*.png

win32:SOURCES+=$$ICONS
win32:Resource=$$ICONS
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
