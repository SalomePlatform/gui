TEMPLATE = app
DESTDIR = ../../bin
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

INCLUDEPATH = ../../include
LIBS += -L../../lib -lSUIT -lQtx

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32

HEADERS = *.h

SOURCES = *.cxx
