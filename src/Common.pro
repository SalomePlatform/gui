CONFIG -= debug release debug_and_release
CONFIG += qt thread dll shared

CONFIG_MODE= $$(CONFIG_MODE)
contains( CONFIG_MODE, debug ) {
  CONFIG += debug
} else {
  CONFIG += release
}

win32:QMAKE_MOC=$(QTDIR)\bin\moc.exe

isEmpty( HEADERS ):HEADERS = *.h
isEmpty( SOURCES ):SOURCES = *.cxx

MOC_DIR = $$(TMP_DIR)/moc

unix {
  OBJECTS_DIR = $$(TMP_DIR)/obj
  contains( TEMPLATE, lib ) {
    DESTDIR = $(LIB_DIR)
  } else {
    DESTDIR = $(BIN_DIR)
  }

  HEADERS_DIR = $$(BUILD_DIR)/include
  INCLUDEPATH += $${HEADERS_DIR}
  LIBS += -L$(LIB_DIR)

  GUIResources = $(RES_DIR)

  includes.files = $$HEADERS
  includes.path = $${HEADERS_DIR}

  INSTALLS += includes
}

win32 {
  OBJECTS_DIR = ../../$$(CONFIG_ID)/$$(CONFIG_MODE)/obj/$$TARGET
  contains( TEMPLATE, vclib ) {
    DESTDIR = $$(SUIT_DIR)/$$(CONFIG_ID)/lib/$$(CONFIG_MODE)
  } else {
    DESTDIR = $$(SUIT_DIR)/$$(CONFIG_ID)/bin/$$(CONFIG_MODE)
  }

  HEADERS_DIR = $$(SUIT_DIR)/$$(CONFIG_ID)/include
  INCLUDEPATH += $${HEADERS_DIR}
  LIBS += -L$$(SUIT_DIR)/$$(CONFIG_ID)/lib/$$(CONFIG_MODE)

  copy_hdr.name = Install ${QMAKE_FILE_IN}
  copy_hdr.commands = type ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
  copy_hdr.output = $${HEADERS_DIR}/${QMAKE_FILE_NAME}
  copy_hdr.input = HEADERS
  QMAKE_EXTRA_COMPILERS += copy_hdr

  GUIResources = $$(SUIT_DIR)\\resources
}



# On Win32, disable optimization and generate debug info in release mode
win32 {
  QMAKE_CXXFLAGS_RELEASE *= /Od /Zi
  QMAKE_LFLAGS_RELEASE *= /DEBUG 
}


