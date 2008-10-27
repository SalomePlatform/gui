CONFIG -= debug release debug_and_release
CONFIG += qt thread dll shared

CONFIG_MODE= $$(CONFIG_MODE)
contains( CONFIG_MODE, debug ) {
  CONFIG += debug
} else {
  CONFIG += release
}

