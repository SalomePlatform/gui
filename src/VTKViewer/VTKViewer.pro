TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

VTKHOME = $$(VTKHOME)
VTK_INCLUDES = $${VTKHOME}/include/vtk

VTK_LIBS = -L$${VTKHOME}/lib/vtk -L$${VTKHOME}/lib/vtk/python -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets   -lGL -L/usr/X11R6/lib -lGLU -L/usr/X11R6/lib -lX11 -lXt

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

INCLUDEPATH += ../../include $${VTK_INCLUDES} $${CAS_CPPFLAGS} ../Qtx ../SUIT
LIBS += -L../../$(CONFIG_ID)/lib -lQtx -lSUIT $${VTK_LIBS} $${CAS_KERNEL}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += VTKVIEWER_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = *.h

SOURCES  = *.cxx

TRANSLATIONS = resources/VTKViewer_images.ts \
               resources/VTKViewer_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
