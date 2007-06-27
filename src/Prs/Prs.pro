TEMPLATE = lib
TARGET = SalomePrs
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PRS_EXPORTS

HEADERS  = SALOME_Prs.h

SOURCES  = SALOME_Prs.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
