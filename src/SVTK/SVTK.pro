TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

VTK_LIBS = -L$$(VTKLIB) -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets   -lGL -L/usr/X11R6/lib -lGLU -L/usr/X11R6/lib -lX11 -lXt

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

OCC_LIBS = 

INCLUDEPATH += ../../include $${CAS_CPPFLAGS} $$(VTKINC) $$(BOOSTINC) ../Qtx ../SUIT ../OBJECT ../Prs ../VTKViewer
LIBS += -L../../$(CONFIG_ID)/lib -lQtx -lSUIT -lOBJECT -lPrs -lVTKViewer $${OCC_LIBS} $${VTK_LIBS}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += SVTK_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = SVTK.h
HEADERS += SVTK_Prs.h
HEADERS += SVTK_Actor.h
HEADERS += SALOME_Actor.h
HEADERS += SVTK_RectPicker.h
HEADERS += SVTK_DeviceActor.h
HEADERS += SVTK_Actor2DWidget.h
HEADERS += SVTK_CaptionActor2DWidget.h
HEADERS += SVTK_NonIsometricDlg.h
HEADERS += SVTK_TextRegionDlg.h
HEADERS += SVTK_UpdateRateDlg.h
HEADERS += SVTK_CubeAxesDlg.h

HEADERS += SVTK_DialogBase.h
HEADERS += SVTK_FontWidget.h
HEADERS += SVTK_CubeAxesActor2D.h
HEADERS += SVTK_Functor.h
HEADERS += SVTK_MainWindow.h
HEADERS += SVTK_View.h
HEADERS += SVTK_ViewManager.h
HEADERS += SVTK_ViewModel.h
HEADERS += SVTK_ViewWindow.h
HEADERS += SVTK_Renderer.h
HEADERS += SVTK_InteractorStyle.h
HEADERS += SVTK_RenderWindowInteractor.h
HEADERS += SVTK_GenericRenderWindowInteractor.h
HEADERS += SVTK_Selector.h
HEADERS += SVTK_Selection.h
HEADERS += SVTK_SelectionEvent.h
HEADERS += SVTK_SpaceMouse.h
HEADERS += SVTK_Event.h
HEADERS += SVTK_ViewModelBase.h
HEADERS += SVTK_SetRotationPointDlg.h
HEADERS += SVTK_Extension.h

SOURCES  = SVTK_Prs.cxx
SOURCES += SVTK_Actor.cxx
SOURCES += SALOME_Actor.cxx
SOURCES += SVTK_RectPicker.cxx
SOURCES += SVTK_DeviceActor.cxx
SOURCES += SVTK_Actor2DWidget.cxx
SOURCES += SVTK_CaptionActor2DWidget.cxx
SOURCES += SVTK_CubeAxesActor2D.cxx
SOURCES += SVTK_NonIsometricDlg.cxx
SOURCES += SVTK_UpdateRateDlg.cxx
SOURCES += SVTK_TextRegionDlg.cxx
SOURCES += SVTK_CubeAxesDlg.cxx
SOURCES += SVTK_DialogBase.cxx
SOURCES += SVTK_FontWidget.cxx
SOURCES += SVTK_Trihedron.cxx
SOURCES += SVTK_MainWindow.cxx
SOURCES += SVTK_View.cxx
SOURCES += SVTK_ViewManager.cxx
SOURCES += SVTK_ViewModel.cxx
SOURCES += SVTK_Renderer.cxx
SOURCES += SVTK_ViewWindow.cxx
SOURCES += SVTK_InteractorStyle.cxx
SOURCES += SVTK_RenderWindowInteractor.cxx
SOURCES += SVTK_GenericRenderWindowInteractor.cxx
SOURCES += SVTK_SpaceMouse.cxx
SOURCES += SVTK_Selector.cxx
SOURCES += SVTK_SetRotationPointDlg.cxx
SOURCES += SVTK_Extension.cxx

TRANSLATIONS = resources/SVTK_images.ts \
               resources/SVTK_msg_en.ts

unix:GUIResources = ../../resources
win32:GUIResources = ..\\..\\resources

lrelease.name = LRELASE ${QMAKE_FILE_IN}
unix:lrelease.commands = $(QTDIR)/$(CONFIG_ID)/bin/lrelease ${QMAKE_FILE_NAME} -qm $${GUIResources}/${QMAKE_FILE_BASE}.qm
win32:lrelease.commands = $(QTDIR)\\$(CONFIG_ID)\\bin\\lrelease ${QMAKE_FILE_NAME} -qm $${GUIResources}\\${QMAKE_FILE_BASE}.qm
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

INSTALLS += includes
