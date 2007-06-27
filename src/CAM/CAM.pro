TEMPLATE = lib
TARGET = CAM
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

INCLUDEPATH += ../../include ../Qtx ../SUIT ../STD
LIBS += -L../../lib -lqtx -lsuit -lstd

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += CAM_EXPORTS

HEADERS  = CAM.h
HEADERS += CAM_Application.h
HEADERS += CAM_DataModel.h
HEADERS += CAM_DataObject.h
HEADERS += CAM_Module.h
HEADERS += CAM_RootObject.h
HEADERS += CAM_Study.h

SOURCES  = CAM_Application.cxx
SOURCES += CAM_DataModel.cxx
SOURCES += CAM_DataObject.cxx
SOURCES += CAM_Module.cxx
SOURCES += CAM_RootObject.cxx
SOURCES += CAM_Study.cxx

TRANSLATIONS = resources/CAM_images.ts \
               resources/CAM_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm resources/*.xml resources/*.ini
resources.path = ../../resources

INSTALLS += includes resources
