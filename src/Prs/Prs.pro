unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32
DEFINES += PRS_EXPORTS
