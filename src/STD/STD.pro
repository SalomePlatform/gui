TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

INCLUDEPATH = ../../include
LIBS += -L../../$(CONFIG_ID)/lib -lSUIT -lQtx

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += STD_EXPORTS

HEADERS = *.h

SOURCES = *.cxx

TRANSLATIONS = resources/STD_images.ts \
               resources/STD_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm resources/*.xml resources/*.ini
resources.path = ../../resources

INSTALLS += includes resources
