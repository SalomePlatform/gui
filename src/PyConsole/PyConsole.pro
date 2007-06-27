TEMPLATE = lib
TARGET = PyConsole
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

PYTHONVER=2.4
PYTHONHOME=$$(PYTHONHOME)
PYTHONINC=$${PYTHONHOME}/include/python$${PYTHONVER}
PYTHONLIB=$${PYTHONHOME}/lib

INCLUDEPATH += ../../include $${PYTHONINC}
INCLUDEPATH += ../Qtx ../SUIT ../PyInterp
unix:LIBS  += -L$${PYTHONLIB} -lpython$${PYTHONVER}
win32:LIBS += /LIBPATH:$$(PYTHONLIB)
LIBS += -L../../lib -lqtx -lsuit -lPyInterp

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PYCONSOLE_EXPORTS

HEADERS  = PyConsole.h
HEADERS += PyConsole_Console.h
HEADERS += PyConsole_Editor.h
HEADERS += PyConsole_Interp.h

SOURCES  = PyConsole_Console.cxx
SOURCES += PyConsole_Editor.cxx
SOURCES += PyConsole_Interp.cxx

TRANSLATIONS = resources/PyConsole_msg_en.ts

includes.files = $$HEADERS
includes.path = ../../include

resources.files = resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
