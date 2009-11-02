win32:TEMPLATE = vclib
unix:TEMPLATE = lib

include(../Common.pro)

LIBS += -lQtx -lSUIT -lSTD

win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32
DEFINES += CAM_EXPORTS

include(../Translations.pro)
