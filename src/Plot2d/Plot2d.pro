TEMPLATE = lib

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

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
LIBS += -L../../$(CONFIG_ID)/lib -lQtx -lSUIT

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PLOT2D_EXPORTS

HEADERS  = *.h

SOURCES  = *.cxx

TRANSLATIONS = resources/Plot2d_msg_en.ts \
               resources/Plot2d_images.ts

ICONS = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
