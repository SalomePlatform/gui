TEMPLATE = app
TARGET = SUITApp
DESTDIR = ../../bin
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

INCLUDEPATH += ../../include ../Qtx ../SUIT
LIBS += -L../../lib -lqtx -lsuit

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32

HEADERS  = SUITApp_Application.h

SOURCES  = SUITApp.cxx
SOURCES += SUITApp_Application.cxx
