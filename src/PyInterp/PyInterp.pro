unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

INCLUDEPATH += $$(PYTHONINC)
unix:LIBS  += -lEvent -L$$(PYTHONLIB) -lpython2.7 
win32:LIBS += -lEvent -L$$(PYTHONLIB)
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
