win32:TEMPLATE = vclib
unix:TEMPLATE = lib

include(../Common.pro)

CONFIG += embed_manifest_exe

win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

QT += xml

win32:DEFINES += WNT WIN32
DEFINES += QTX_EXPORTS
