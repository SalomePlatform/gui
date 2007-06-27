TEMPLATE = lib
TARGET = SPlot2d
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

QWTHOME=$$(QWTHOME)
QWTINC=$${QWTHOME}/include
QWTLIB=$${QWTHOME}/lib

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

INCLUDEPATH += ../../include $${CAS_CPPFLAGS} ../../include $${QWTINC} $${BOOST_CPPFLAGS}
INCLUDEPATH += ../Qtx ../SUIT ../Plot2d ../Prs ../OBJECT
unix:LIBS  += -L$${QWTLIB} -lqwt
win32:LIBS += /LIBPATH:$$(QWTLIB)
LIBS += -L../../lib -lsuit -lPlot2d -lSalomePrs

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += SPLOT2D_EXPORTS

HEADERS  = SPlot2d.h
HEADERS  = SPlot2d_Curve.h
HEADERS  = SPlot2d_Prs.h
HEADERS  = SPlot2d_ViewModel.h
HEADERS  = SPlot2d_ViewWindow.h

SOURCES   = SPlot2d_Curve.cxx
SOURCES  += SPlot2d_Curve.cxx
SOURCES  += SPlot2d_Prs.cxx
SOURCES  += SPlot2d_ViewModel.cxx
SOURCES  += SPlot2d_ViewWindow.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
