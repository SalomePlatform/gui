unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

win32:QMAKE_MOC=$(QTDIR)\bin\moc.exe

DESTDIR = ../../$(CONFIG_ID)/lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../$(CONFIG_ID)/obj/$$TARGET

INCLUDEPATH += ../../include $$(PYTHONINC)
unix:LIBS  += -L$$(PYTHONLIB) -lpython2.5 -L../../$(CONFIG_ID)/lib -lEvent
win32:LIBS += -L$$(PYTHONLIB)
win32:CONFIG_MODE= $$(CONFIG_MODE)
contains( CONFIG_MODE, debug ) {
  win32:LIBS += -lpython25_d
} else {
  win32:LIBS += -lpython25
}
win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32 HAVE_DEBUG_PYTHON
DEFINES += PYINTERP_EXPORTS

HEADERS = *.h

SOURCES = *.cxx

win32:copy_hdr.name = Install ${QMAKE_FILE_IN}
win32:copy_hdr.commands = type ${QMAKE_FILE_IN} > ../../include/${QMAKE_FILE_BASE}.h
win32:copy_hdr.output = ../../include/${QMAKE_FILE_BASE}.h
win32:copy_hdr.input = HEADERS
win32:QMAKE_EXTRA_COMPILERS += copy_hdr

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
