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

#--------------------------------------------------------------------------------------
unix {
  install_binaries.files = $$(BIN_DIR) $$(LIB_DIR)
  install_binaries.path = $$(TARGET_DIR)/$$(config_id)
  INSTALLS *= install_binaries

  install_resources.files = $$(RES_DIR)
  install_resources.path = $$(TARGET_DIR)
  INSTALLS *= install_resources

  install_config_run.files = ./run.config/*.*
  install_config_run.path = $$(TARGET_DIR)
  INSTALLS *= install_config_run

  install_addons.files = $$(ROOT_DIR)/launcher_config.py $$(ROOT_DIR)/launcher_engine.py $$(ROOT_DIR)/launcher_env.py
  install_addons.path = $$(TARGET_DIR)
  INSTALLS *= install_addons
}

#--------------------------------------------------------------------------------------
