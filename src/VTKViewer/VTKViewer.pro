TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

VTK_LIBS = -L$$(VTKLIB) -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets   -lGL -L/usr/X11R6/lib -lGLU -L/usr/X11R6/lib -lX11 -lXt

CAS_CPPFLAGS = $(CASINC)

CAS_KERNEL = -L$(CASLIB) -lTKernel

INCLUDEPATH += ../../include $$(VTKINC) $${CAS_CPPFLAGS} ../Qtx ../SUIT
LIBS += -L../../$(CONFIG_ID)/lib -lQtx -lSUIT $${VTK_LIBS} $${CAS_KERNEL}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += VTKVIEWER_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

LIB_64 {
	DEFINES += _OCC64
}

HEADERS  = VTKViewer.h
HEADERS += VTKViewer_CellLocationsArray.h
HEADERS += VTKViewer_Actor.h
HEADERS += VTKViewer_ExtractUnstructuredGrid.h
HEADERS += VTKViewer_ConvexTool.h
HEADERS += VTKViewer_Filter.h
HEADERS += VTKViewer_GeometryFilter.h
HEADERS += VTKViewer_AppendFilter.h
HEADERS += VTKViewer_Algorithm.h
HEADERS += VTKViewer_InteractorStyle.h
HEADERS += VTKViewer_RenderWindow.h
HEADERS += VTKViewer_RenderWindowInteractor.h
HEADERS += VTKViewer_ShrinkFilter.h
HEADERS += VTKViewer_TransformFilter.h
HEADERS += VTKViewer_Transform.h
HEADERS += VTKViewer_Trihedron.h
HEADERS += VTKViewer_Utilities.h
HEADERS += VTKViewer_ViewManager.h
HEADERS += VTKViewer_ViewModel.h
HEADERS += VTKViewer_ViewWindow.h
HEADERS += VTKViewer_Functor.h

SOURCES  = VTKViewer_CellLocationsArray.cxx
SOURCES += VTKViewer_Actor.cxx
SOURCES += VTKViewer_ExtractUnstructuredGrid.cxx
SOURCES += VTKViewer_Filter.cxx
SOURCES += VTKViewer_GeometryFilter.cxx
SOURCES += VTKViewer_AppendFilter.cxx
SOURCES += VTKViewer_InteractorStyle.cxx
SOURCES += VTKViewer_RenderWindow.cxx
SOURCES += VTKViewer_RenderWindowInteractor.cxx
SOURCES += VTKViewer_ShrinkFilter.cxx
SOURCES += VTKViewer_Transform.cxx
SOURCES += VTKViewer_TransformFilter.cxx
SOURCES += VTKViewer_Trihedron.cxx
SOURCES += VTKViewer_Utilities.cxx
SOURCES += VTKViewer_ViewManager.cxx
SOURCES += VTKViewer_ViewModel.cxx
SOURCES += VTKViewer_ConvexTool.cxx
SOURCES += VTKViewer_ViewWindow.cxx

message ( "Sources: $$SOURCES " )

TRANSLATIONS = resources/VTKViewer_images.ts \
               resources/VTKViewer_msg_en.ts

unix:GUIResources = ../../resources
win32:GUIResources = ..\\..\\resources

lrelease.name = LRELASE ${QMAKE_FILE_IN}
unix:lrelease.commands = $(QTDIR)/bin/lrelease ${QMAKE_FILE_NAME} -qm $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.commands = $(QTDIR)\\bin\\lrelease ${QMAKE_FILE_NAME} -qm $${GUIResources}\\${QMAKE_FILE_BASE}.qm
unix:lrelease.output = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.output = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.input = TRANSLATIONS
unix:lrelease.clean = $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.clean = $${GUIResources}\\${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS
resources.path = ../../resources

INSTALLS += includes resources
