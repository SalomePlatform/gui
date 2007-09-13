TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

INCLUDEPATH = ../../include
win32:LIBS += ../../lib/suit.lib ../../lib/qtx.lib
unix:LIBS += -L../../$(CONFIG_ID)/lib -lSUIT -lQtx

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += LOGWINDOW_EXPORTS

HEADERS = *.h

SOURCES = *.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
