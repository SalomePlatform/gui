TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

VTKHOME = $$(VTKHOME)
VTK_INCLUDES = $${VTKHOME}/include/vtk

VTK_LIBS = -L$${VTKHOME}/lib/vtk -L$${VTKHOME}/lib/vtk/python -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets   -lGL -L/usr/X11R6/lib -lGLU -L/usr/X11R6/lib -lX11 -lXt

INCLUDEPATH = ../../include $${VTK_INCLUDES}
LIBS += -L../../$(CONFIG_ID)/lib $${VTK_LIBS} -lSUIT -lQtx -lTableViewer -lVTKViewer

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += STD_EXPORTS

HEADERS = *.h

SOURCES = *.cxx

TRANSLATIONS = resources/STD_images.ts \
               resources/STD_msg_en.ts

ICONS   = resources/*.png \
	  STD.xml

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm resources/*.xml resources/*.ini
resources.path = ../../resources

INSTALLS += includes resources
