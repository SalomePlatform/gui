TEMPLATE = lib
TARGET = LogWindow
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

INCLUDEPATH += ../../include ../Qtx ../SUIT
LIBS += -L../../lib -lqtx -lsuit

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += LOGWINDOW_EXPORTS

HEADERS = LogWindow.h

SOURCES = LogWindow.cxx

TRANSLATIONS = resources/LogWindow_msg_en.ts

includes.files = $$HEADERS
includes.path = ../../include

resources.files = resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
