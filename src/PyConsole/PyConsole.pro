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

includes.files = $$HEADERS
includes.path = ../../include

resources.files = resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
