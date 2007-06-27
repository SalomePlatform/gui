TEMPLATE = lib
TARGET = SalomeObject
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_LDPATH = -L$${CASROOT}/Linux/lib -lTKV3d

INCLUDEPATH += ../../include $${CAS_CPPFLAGS}
LIBS += $${CAS_LDPATH}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = SALOME_InteractiveObject.hxx
HEADERS += Handle_SALOME_InteractiveObject.hxx
HEADERS += SALOME_AISShape.hxx
HEADERS += Handle_SALOME_AISShape.hxx
HEADERS += SALOME_AISObject.hxx
HEADERS += Handle_SALOME_AISObject.hxx
HEADERS += SALOME_ListIO.hxx
HEADERS += SALOME_ListIteratorOfListIO.hxx
HEADERS += Handle_SALOME_ListNodeOfListIO.hxx
HEADERS += SALOME_ListNodeOfListIO.hxx
HEADERS += Handle_SALOME_Filter.hxx
HEADERS += SALOME_Filter.hxx
HEADERS += Handle_SALOME_TypeFilter.hxx
HEADERS += SALOME_TypeFilter.hxx
HEADERS += SALOME_DataMapOfIOMapOfInteger.hxx
HEADERS += SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger.hxx
HEADERS += Handle_SALOME_DataMapNodeOfDataMapOfIOMapOfInteger.hxx
HEADERS += SALOME_Selection.h
HEADERS += SALOME_AISObject.ixx
HEADERS += SALOME_AISObject.jxx
HEADERS += SALOME_AISShape.ixx
HEADERS += SALOME_AISShape.jxx
HEADERS += SALOME_Filter.ixx
HEADERS += SALOME_Filter.jxx
HEADERS += SALOME_InteractiveObject.ixx
HEADERS += SALOME_InteractiveObject.jxx
HEADERS += SALOME_TypeFilter.ixx
HEADERS += SALOME_TypeFilter.jxx
HEADERS += SALOME_DataMapNodeOfDataMapOfIOMapOfInteger.hxx

SOURCES  = SALOME_InteractiveObject.cxx
SOURCES += SALOME_AISShape.cxx
SOURCES += SALOME_AISObject.cxx
SOURCES += SALOME_ListIO_0.cxx
SOURCES += SALOME_ListIteratorOfListIO_0.cxx
SOURCES += SALOME_ListNodeOfListIO_0.cxx
SOURCES += SALOME_Filter.cxx
SOURCES += SALOME_TypeFilter.cxx
SOURCES += SALOME_DataMapOfIOMapOfInteger_0.cxx
SOURCES += SALOME_DataMapNodeOfDataMapOfIOMapOfInteger_0.cxx
SOURCES += SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger_0.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
