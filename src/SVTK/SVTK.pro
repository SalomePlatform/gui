unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

unix:VTK_LIBS = -L$$(VTKLIB) -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets   -lGL -L/usr/X11R6/lib -lGLU -L/usr/X11R6/lib -lX11 -lXt
win32:VTK_LIBS = -L$$(VTKLIB) -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets -lopengl32

CAS_CPPFLAGS = $(CASINC)

unix:OCC_LIBS = 
win32:OCC_LIBS = -L$(CASLIB) -lTKernel

INCLUDEPATH += $${CAS_CPPFLAGS} $$(VTKINC) $$(BOOSTINC) ../Qtx ../SUIT ../OBJECT ../Prs ../VTKViewer
LIBS += -lQtx -lSUIT -lOBJECT -lPrs -lVTKViewer $${OCC_LIBS} $${VTK_LIBS}
win32: LIBS += -lTKernel
win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32 
DEFINES += SVTK_EXPORTS $(CASDEFINES) OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

include(../Translations.pro)
include(../Resources.pro)
