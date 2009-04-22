unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

INCLUDEPATH += $$(QWTINC) $$(PYTHONINC)
INCLUDEPATH += ../Qtx ../SUIT
unix:LIBS  += -L$$(QWTLIB) -lqwt
win32:LIBS += /LIBPATH:$$(QWTLIB) -lqwt5
LIBS += -lQtx -lSUIT
win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32 QWT_DLL
DEFINES += PLOT2D_EXPORTS

include(../Translations.pro)
include(../Resources.pro)