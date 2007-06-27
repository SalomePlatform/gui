TEMPLATE = lib
TARGET = QDS
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

INCLUDEPATH += ../../include $${CAS_CPPFLAGS} ../Qtx ../DDS
LIBS += -L../../lib -lqtx -lDDS $${CAS_KERNEL}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += QDS_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = QDS.h
HEADERS += QDS_CheckBox.h
HEADERS += QDS_ComboBox.h
HEADERS += QDS_Datum.h
HEADERS += QDS_LineEdit.h
HEADERS += QDS_SpinBox.h
HEADERS += QDS_SpinBoxDbl.h
HEADERS += QDS_TextEdit.h
HEADERS += QDS_Validator.h
HEADERS += QDS_RadioBox.h
#HEADERS += QDS_Table.h

SOURCES  = QDS.cxx
SOURCES += QDS_CheckBox.cxx
SOURCES += QDS_ComboBox.cxx
SOURCES += QDS_Datum.cxx
SOURCES += QDS_LineEdit.cxx
SOURCES += QDS_SpinBox.cxx
SOURCES += QDS_SpinBoxDbl.cxx
SOURCES += QDS_TextEdit.cxx
SOURCES += QDS_Validator.cxx
SOURCES += QDS_RadioBox.cxx
#SOURCES += QDS_Table.cxx

TRANSLATIONS = resources/QDS_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
