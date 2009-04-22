unix:TEMPLATE = app
win32:TEMPLATE = vcapp

include(../Common.pro)
CONFIG -= embed_manifest_exe

LIBS += -lSUIT -lQtx -lStyle -lObjBrowser
win32:LIBS *= -L$(QTLIB) -lUserenv
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32
DEFINES += DISABLE_TESTRECORDER

include(../Translations.pro)

