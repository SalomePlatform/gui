TEMPLATE = lib
TARGET = Plot2d
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

PYTHONVER=2.4
PYTHONHOME=$$(PYTHONHOME)
PYTHONINC=$${PYTHONHOME}/include/python$${PYTHONVER}

QWTHOME=$$(QWTHOME)
QWTINC=$${QWTHOME}/include
QWTLIB=$${QWTHOME}/lib

INCLUDEPATH += ../../include $${QWTINC} $${PYTHONINC}
INCLUDEPATH += ../Qtx ../SUIT
unix:LIBS  += -L$${QWTLIB} -lqwt
win32:LIBS += /LIBPATH:$$(QWTLIB)
LIBS += -L../../lib -lqtx -lsuit

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PLOT2D_EXPORTS

HEADERS  = Plot2d.h
HEADERS += Plot2d_Curve.h
HEADERS += Plot2d_FitDataDlg.h
HEADERS += Plot2d_Prs.h
HEADERS += Plot2d_SetupViewDlg.h
HEADERS += Plot2d_ViewFrame.h
HEADERS += Plot2d_ViewManager.h
HEADERS += Plot2d_ViewModel.h
HEADERS += Plot2d_ViewWindow.h
HEADERS += Plot2d_SetupCurveDlg.h
HEADERS += Plot2d_ToolTip.h

SOURCES  = Plot2d_Curve.cxx
SOURCES += Plot2d_FitDataDlg.cxx
SOURCES += Plot2d_Prs.cxx
SOURCES += Plot2d_SetupViewDlg.cxx
SOURCES += Plot2d_ViewFrame.cxx
SOURCES += Plot2d_ViewManager.cxx
SOURCES += Plot2d_ViewModel.cxx
SOURCES += Plot2d_ViewWindow.cxx
SOURCES += Plot2d_SetupCurveDlg.cxx
SOURCES += Plot2d_ToolTip.cxx

TRANSLATIONS = resources/Plot2d_msg_en.ts \
               resources/Plot2d_images.ts

ICONS = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
