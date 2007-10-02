TEMPLATE = lib

DESTDIR =  ../../$(CONFIG_ID)/lib
MOC_DIR =  ../../moc
OBJECTS_DIR =  ../../$(CONFIG_ID)/obj/$$TARGET

INCLUDEPATH =  ../../include
LIBS += -L ../../$(CONFIG_ID)/lib -lQtx

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += STYLE_SALOME_EXPORTS

HEADERS = *.h

SOURCES = *.cxx

includes.files = $$HEADERS
includes.path =  ../../include

INSTALLS += includes
