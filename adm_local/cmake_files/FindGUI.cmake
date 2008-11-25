
SET(GUI_CXXFLAGS -I${GUI_ROOT_DIR}/include/salome)

FIND_LIBRARY(qtx qtx ${GUI_ROOT_DIR}/lib/salome)
FIND_LIBRARY(suit suit ${GUI_ROOT_DIR}/lib/salome)
FIND_LIBRARY(SalomeApp SalomeApp ${GUI_ROOT_DIR}/lib/salome)
FIND_LIBRARY(SalomeSession SalomeSession ${GUI_ROOT_DIR}/lib/salome)
