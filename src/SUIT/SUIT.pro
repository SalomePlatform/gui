unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

LIBS += -lQtx -lObjBrowser
win32:LIBS *= -L$(QTLIB) -lUserenv
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui $(QTINC)\QtXml

win32:DEFINES += WNT WIN32
DEFINES += SUIT_EXPORTS DISABLE_TESTRECORDER

include(../Translations.pro)
