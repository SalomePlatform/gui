TEMPLATE = lib
TARGET = suit
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

INCLUDEPATH += ../../include ../Qtx
LIBS += -L../../lib -lqtx

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += SUIT_EXPORTS

HEADERS  = SUIT.h
HEADERS += SUIT_Accel.h
HEADERS += SUIT_ActionOperation.h
HEADERS += SUIT_Application.h
HEADERS += SUIT_DataObject.h
HEADERS += SUIT_DataObjectIterator.h
HEADERS += SUIT_DataObjectKey.h
HEADERS += SUIT_DataOwner.h
HEADERS += SUIT_Desktop.h
HEADERS += SUIT_ExceptionHandler.h
HEADERS += SUIT_FileDlg.h
HEADERS += SUIT_FileValidator.h
HEADERS += SUIT_MessageBox.h
HEADERS += SUIT_Operation.h
HEADERS += SUIT_OverrideCursor.h
HEADERS += SUIT_PopupClient.h
HEADERS += SUIT_ResourceMgr.h
HEADERS += SUIT_SelectionFilter.h
HEADERS += SUIT_SelectionMgr.h
HEADERS += SUIT_Selector.h
HEADERS += SUIT_Session.h
HEADERS += SUIT_SmartPtr.h
HEADERS += SUIT_Study.h
HEADERS += SUIT_Tools.h
HEADERS += SUIT_TreeSync.h
HEADERS += SUIT_ViewManager.h
HEADERS += SUIT_ViewModel.h
HEADERS += SUIT_ViewWindow.h

SOURCES  = SUIT_Accel.cxx
SOURCES += SUIT_ActionOperation.cxx
SOURCES += SUIT_Application.cxx
SOURCES += SUIT_DataObject.cxx
SOURCES += SUIT_DataObjectIterator.cxx
SOURCES += SUIT_DataObjectKey.cxx
SOURCES += SUIT_DataOwner.cxx
SOURCES += SUIT_Desktop.cxx
SOURCES += SUIT_ExceptionHandler.cxx
SOURCES += SUIT_FileDlg.cxx
SOURCES += SUIT_FileValidator.cxx
SOURCES += SUIT_MessageBox.cxx
SOURCES += SUIT_Operation.cxx
SOURCES += SUIT_OverrideCursor.cxx
SOURCES += SUIT_PopupClient.cxx
SOURCES += SUIT_ResourceMgr.cxx
SOURCES += SUIT_SelectionFilter.cxx
SOURCES += SUIT_SelectionMgr.cxx
SOURCES += SUIT_Selector.cxx
SOURCES += SUIT_Session.cxx
SOURCES += SUIT_Study.cxx
SOURCES += SUIT_Tools.cxx
SOURCES += SUIT_ViewManager.cxx
SOURCES += SUIT_ViewModel.cxx
SOURCES += SUIT_ViewWindow.cxx

TRANSLATIONS = resources/SUIT_images.ts \
               resources/SUIT_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
