unix:TEMPLATE = subdirs
win32:TEMPLATE = vcsubdirs

CONFIG += ordered

win32:CONFIG += embed_manifest_exe
CONFIG -= debug release debug_and_release
CONFIG += qt thread dll shared

CONFIG_MODE= $$(CONFIG_MODE)
contains( CONFIG_MODE, debug ) {
  CONFIG += debug
} else {
  CONFIG += release
}

SUBDIRS = src/Qtx \
          src/Style \
	  src/ObjBrowser \
          src/SUIT \
          src/SUITApp \
          src/LogWindow \
          src/Event \
          src/PyInterp \
          src/PyConsole \
	  src/Plot2d \
	  src/VTKViewer \
	  src/HTMLService \
	  src/TableViewer \
	  src/OBJECT \
	  src/Prs \
	  src/SVTK \
          src/STD \
          src/CAM
