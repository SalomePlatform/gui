TEMPLATE = lib
TARGET = Event

include(../Common.pro)

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

INCLUDEPATH += ../../include
LIBS += 

win32:DEFINES += WIN32 
DEFINES += EVENT_EXPORTS

HEADERS  = Event.h
HEADERS += SALOME_Event.h

SOURCES  = SALOME_Event.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
