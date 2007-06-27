TEMPLATE = lib
TARGET = SOCC
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

INCLUDEPATH += ../../include $${CAS_CPPFLAGS} $${BOOST_CPPFLAGS} ../Qtx ../SUIT ../OBJECT ../Prs ../OCCViewer
LIBS += -L../../lib -lqtx -lsuit -lSalomeObject -lSalomePrs -lOCCViewer

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += SOCC_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = SOCC.h
HEADERS += SOCC_ViewModel.h
HEADERS += SOCC_Prs.h
HEADERS += SOCC_ViewWindow.h

SOURCES  = SOCC_ViewModel.cxx
SOURCES += SOCC_Prs.cxx
SOURCES += SOCC_ViewWindow.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
