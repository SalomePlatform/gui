TEMPLATE = lib
TARGET = caf
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

INCLUDEPATH += ../../include ../Qtx ../SUIT ../STD
LIBS += -L../../lib -lqtx -lsuit -lstd

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += CAF_EXPORTS

HEADERS  = CAF.h
HEADERS += CAF_Application.h
HEADERS += CAF_Operation.h
HEADERS += CAF_Study.h
HEADERS += CAF_Tools.h

SOURCES  = CAF_Application.cxx
SOURCES += CAF_Operation.cxx
SOURCES += CAF_Study.cxx
SOURCES += CAF_Tools.cxx

TRANSLATIONS = resources/CAF_images.ts \
               resources/CAF_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm resources/*.xml resources/*.ini
resources.path = ../../resources

INSTALLS += includes resources
