TEMPLATE = lib
TARGET = PyInterp
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

PYTHONVER=2.4
PYTHONHOME=$$(PYTHONHOME)
PYTHONINC=$${PYTHONHOME}/include/python$${PYTHONVER}
PYTHONLIB=$${PYTHONHOME}/lib

INCLUDEPATH += ../../include $${PYTHONINC}
unix:LIBS  += -L$${PYTHONLIB} -lpython$${PYTHONVER}
win32:LIBS += /LIBPATH:$${PYTHONLIB}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PYINTERP_EXPORTS

HEADERS  = PyInterp.h
HEADERS += PyInterp_Dispatcher.h
HEADERS += PyInterp_Watcher.h
HEADERS += PyInterp_Interp.h

SOURCES  = PyInterp_Dispatcher.cxx
SOURCES += PyInterp_Interp.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
