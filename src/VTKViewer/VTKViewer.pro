unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

unix:VTK_LIBS = -L$$(VTKLIB) -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets   -lGL -L/usr/X11R6/lib -lGLU -L/usr/X11R6/lib -lX11 -lXt
win32:VTK_LIBS = -L$$(VTKLIB) -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets

CAS_CPPFLAGS = $(CASINC)

CAS_KERNEL = -L$(CASLIB) -lTKernel

INCLUDEPATH += $$(VTKINC) $${CAS_CPPFLAGS} ../Qtx ../SUIT
LIBS += -lQtx -lSUIT $${VTK_LIBS} $${CAS_KERNEL}
win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32 
DEFINES += VTKVIEWER_EXPORTS $(CASDEFINES) OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=3 OCC_VERSION_MAINTENANCE=5 LIN LINTEL CSFDB No_Exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

include(../Translations.pro)
include(../Resources.pro)
