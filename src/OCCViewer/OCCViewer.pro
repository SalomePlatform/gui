unix:TEMPLATE = lib
win32:TEMPLATE = vclib

include(../Common.pro)

CAS_CPPFLAGS = $(CASINC)

CAS_KERNEL = -L$(CASLIB) -lTKernel

CAS_VIEWER = -L$(CASLIB) -lTKV3d -lTKService


INCLUDEPATH += $${CAS_CPPFLAGS}
LIBS += 
LIBS += $${CAS_KERNEL} $${CAS_VIEWER}

LIBS *= -lSUIT -lQtx -lTKMath -lTKG3d -lTKGeomBase

win32:DEFINES += WNT WIN32
DEFINES += OCCVIEWER_EXPORTS DISABLE_TESTRECORDER
DEFINES += HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

include(../Translations.pro)
include(../Resources.pro)
