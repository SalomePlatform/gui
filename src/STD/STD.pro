TEMPLATE = lib
TARGET = std
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

INCLUDEPATH += ../../include ../Qtx ../SUIT
LIBS += -L../../lib -lqtx -lsuit

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += STD_EXPORTS

HEADERS  = STD.h
HEADERS += STD_Application.h
HEADERS += STD_MDIDesktop.h
HEADERS += STD_SDIDesktop.h
HEADERS += STD_TabDesktop.h

SOURCES  = STD_Application.cxx
SOURCES += STD_MDIDesktop.cxx
SOURCES += STD_SDIDesktop.cxx
SOURCES += STD_TabDesktop.cxx

TRANSLATIONS = resources/STD_images.ts \
               resources/STD_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm resources/*.xml resources/*.ini
resources.path = ../../resources

INSTALLS += includes resources
