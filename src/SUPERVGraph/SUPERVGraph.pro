TEMPLATE = lib
TARGET = SUPERVGraph
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

QT_INCLUDES = $$(QTDIR)/include $$(QTDIR)/include/QtCore $$(QTDIR)/include/QtGui $$(QTDIR)/include/QtOpenGL $$(QTDIR)/include/QtXml

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

PYTHON_INCLUDES = $$(PYTHONHOME)/include/python2.4

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

QT_MT_LIBS = -L$$(QTDIR)/lib -lQtCore -lQtXml -lQtGui -lQtOpenGL

INCLUDEPATH += $${QT_INCLUDES} $${CAS_CPPFLAGS} $${PYTHON_INCLUDES} $${BOOST_CPPFLAGS} ../Qtx ../SUIT ../OBJECT

LIBS += $${QT_MT_LIBS} -L../../lib -lsuit

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += SUPERVGRAPH_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = SUPERVGraph_ViewFrame.h
HEADERS += SUPERVGraph_ViewManager.h
HEADERS += SUPERVGraph_ViewModel.h
HEADERS += SUPERVGraph.h

SOURCES  = SUPERVGraph.cxx
SOURCES += SUPERVGraph_ViewFrame.cxx
SOURCES += SUPERVGraph_ViewManager.cxx
SOURCES += SUPERVGraph_ViewModel.cxx

TRANSLATIONS = resources/SUPERVGraph_images.ts \
               resources/SUPERVGraph_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
