unix:TEMPLATE = lib
win32:TEMPLATE = vclib
TARGET = Event

include(../Common.pro)

win32:LIBS *= -L$(QTLIB)
win32:INCLUDEPATH *= $(QTINC) $(QTINC)\QtCore $(QTINC)\QtGui

win32:DEFINES += WNT WIN32

DEFINES += EVENT_EXPORTS
