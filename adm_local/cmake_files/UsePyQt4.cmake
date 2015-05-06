# Copyright (C) 2012-2015  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author: Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

####################################################################
#
# _PYQT4_WRAP_GET_UNIQUE_TARGET_NAME: internal function
# 
# Used to generate unique custom target name for usage in
# PYQT4_WRAP_UIC macro.
#
# USAGE: _PYQT4_WRAP_GET_UNIQUE_TARGET_NAME(prefix unique_name)
#
# ARGUMENTS:
#   prefix [in] prefix for the name
#   unique_name [out] unique name generated by function
#
####################################################################
FUNCTION(_PYQT4_WRAP_GET_UNIQUE_TARGET_NAME name unique_name)
   SET(_propertyName "_PYQT4_WRAP_UNIQUE_COUNTER_${name}")
   GET_PROPERTY(_currentCounter GLOBAL PROPERTY "${_propertyName}")
   IF(NOT _currentCounter)
      SET(_currentCounter 1)
   ENDIF()
   SET(${unique_name} "${name}_${_currentCounter}" PARENT_SCOPE)
   MATH(EXPR _currentCounter "${_currentCounter} + 1")
   SET_PROPERTY(GLOBAL PROPERTY ${_propertyName} ${_currentCounter} )
ENDFUNCTION()

####################################################################
#
# PYQT4_WRAP_UIC macro
#
# Create Python modules by processing input *.ui (Qt designer) files with
# PyQt4 pyuic4 tool.
#
# USAGE: PYQT4_WRAP_UIC(output_files pyuic_files)
#
# ARGUMENTS:
#   output_files [out] variable where output file names are listed to
#   pyuic_files  [in]  list of *.ui files
# 
# NOTES:
#   - Input files are considered relative to the current source directory.
#   - Output files are generated in the current build directory.
#   - Macro automatically adds custom build target to generate output files
# 
####################################################################
MACRO(PYQT4_WRAP_UIC outfiles)

 IF(NOT WIN32)

  FOREACH(_input ${ARGN})
    GET_FILENAME_COMPONENT(_input_name ${_input} NAME)
    STRING(REPLACE ".ui" "_ui.py" _input_name ${_input_name})
    SET(_output ${CMAKE_CURRENT_BINARY_DIR}/${_input_name})
    ADD_CUSTOM_COMMAND(
      OUTPUT ${_output}
      COMMAND ${PYQT_PYUIC_PATH} -o ${_output} ${CMAKE_CURRENT_SOURCE_DIR}/${_input}
      MAIN_DEPENDENCY ${_input}
      )
    SET(${outfiles} ${${outfiles}} ${_output})
  ENDFOREACH()
  _PYQT4_WRAP_GET_UNIQUE_TARGET_NAME(BUILD_UI_PY_FILES _uniqueTargetName)
  ADD_CUSTOM_TARGET(${_uniqueTargetName} ALL DEPENDS ${${outfiles}})

 ELSE(NOT WIN32)
####
# ANA: Workaround for the Microsoft Visual Studio 2010. Seems there is a bug in 
# the Microsoft Visual Studio 2010 or CMake 2.8.10.2: custom target doesn't work 
# for the list of the dependencies. It works only for the first dependency in the 
# list. So generate separate target for the each input file. This problem will be 
#investigated in the future.
####

  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
  _PYQT4_WRAP_GET_UNIQUE_TARGET_NAME(BUILD_UI_PY_FILES _uniqueTargetName)
  ADD_CUSTOM_TARGET(${_uniqueTargetName} ALL)
  FOREACH(_input ${ARGN})
    GET_FILENAME_COMPONENT(_input_name ${_input} NAME)
    STRING(REPLACE ".ui" "_ui.py" _input_name ${_input_name})
    SET(_output ${CMAKE_CURRENT_BINARY_DIR}/${_input_name})
    _PYQT4_WRAP_GET_UNIQUE_TARGET_NAME(BUILD_UI_PY_FILES _TgName)
    ADD_CUSTOM_TARGET(${_TgName} ${PYQT_PYUIC_PATH} -o ${_output} ${CMAKE_CURRENT_SOURCE_DIR}/${_input}
      DEPENDS ${_input}
      )
    SET_TARGET_PROPERTIES(${_TgName} PROPERTIES FOLDER PYQT4_WRAP_UIC_TARGETS)
    ADD_DEPENDENCIES(${_uniqueTargetName} DEPEND ${_TgName})
    SET(${outfiles} ${${outfiles}} ${_output})
  ENDFOREACH()
 ENDIF(NOT WIN32)
ENDMACRO(PYQT4_WRAP_UIC)

####################################################################
#
# PYQT4_WRAP_SIP macro
#
# Generate C++ wrappings for *.sip files by processing them with sip.
#
# USAGE: PYQT4_WRAP_SIP(output_files sip_files)
#
# ARGUMENTS:
#   output_files [out] variable where output file names are listed to
#   sip_files    [in]  list of *.sip files
# 
# NOTES:
#   - Input files are considered relative to the current source directory.
#   - Output files are generated in the current build directory.
#   - This version of macro requires class(es) definition in the 
#     *.sip file to be started on a new line without any preceeding characters.
#
# TODO:
#   - Check if dependency of static sources on generated headers works properly:
#     if header is changed, dependant sources should be recompiled.
# 
####################################################################
MACRO(PYQT4_WRAP_SIP outfiles)
  FOREACH(_input ${ARGN})
    FILE(STRINGS ${_input} _sip_modules REGEX "%Module")
    FILE(STRINGS ${_input} _sip_classes REGEX "^class ")
    SET(_output)
    FOREACH(_sip_module ${_sip_modules})
      STRING(REGEX MATCH ".*%Module *\\( *name=.*\\).*" _mod_name "${_sip_module}")
      IF (_mod_name)
	STRING(REGEX REPLACE ".*%Module *\\( *name=(.*).*\\).*" "\\1" _mod_name ${_sip_module})
      ELSE()
	STRING(REGEX REPLACE ".*%Module *(.*)" "\\1" _mod_name ${_sip_module})
      ENDIF()
      SET(_mod_header "sipAPI${_mod_name}.h")
      SET(_mod_source "sip${_mod_name}cmodule${PYQT_CXX_EXT}")
      LIST(APPEND _output ${CMAKE_CURRENT_BINARY_DIR}/${_mod_source})
      SET(${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${_mod_source})
    ENDFOREACH()
    FOREACH(_sip_class ${_sip_classes})
      STRING(REGEX MATCH ".*class +.* *:" _class_name "${_sip_class}")
      IF (_class_name)
	STRING(REGEX REPLACE ".*class +(.*) *:.*" "\\1" _class_name ${_sip_class})
      ELSE()
	STRING(REGEX REPLACE ".*class *(.*)" "\\1" _class_name ${_sip_class})
      ENDIF()
      STRING(STRIP ${_class_name} _class_name)
      SET(_class_source "sip${_mod_name}${_class_name}${PYQT_CXX_EXT}")
      LIST(APPEND _output ${CMAKE_CURRENT_BINARY_DIR}/${_class_source})
      SET(${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${_class_source})
    ENDFOREACH()
    ADD_CUSTOM_COMMAND(
      OUTPUT ${_output}
      COMMAND ${SIP_EXECUTABLE} ${PYQT_SIPFLAGS} ${CMAKE_CURRENT_SOURCE_DIR}/${_input}
      MAIN_DEPENDENCY ${_input}
      )
  ENDFOREACH()
ENDMACRO(PYQT4_WRAP_SIP)


####################################################################
#
# PYQT4_WRAP_QRC macro
#
# Generate Python wrappings for *.qrc files by processing them with pyrcc4.
#
# USAGE: PYQT4_WRAP_QRC(output_files qrc_files)
#
# ARGUMENTS:
#   output_files [out] variable where output file names are listed to
#   qrc_files  [in]  list of *.qrc files
# 
# NOTES:
#   - Input files are considered relative to the current source directory.
#   - Output files are generated in the current build directory.
#   - Macro automatically adds custom build target to generate output files
# 
####################################################################

MACRO(PYQT4_WRAP_QRC outfiles)
  FOREACH(_input ${ARGN})
    GET_FILENAME_COMPONENT(_input_name ${_input} NAME)
    STRING(REPLACE ".qrc" "_qrc.py" _input_name ${_input_name})
    SET(_output ${CMAKE_CURRENT_BINARY_DIR}/${_input_name})
    ADD_CUSTOM_COMMAND(
      OUTPUT ${_output}
      COMMAND ${PYQT_PYRCC_PATH} -o ${_output} ${CMAKE_CURRENT_SOURCE_DIR}/${_input}
      MAIN_DEPENDENCY ${_input}
      )
    SET(${outfiles} ${${outfiles}} ${_output})
  ENDFOREACH()
  _PYQT4_WRAP_GET_UNIQUE_TARGET_NAME(BUILD_QRC_PY_FILES _uniqueTargetName)
  ADD_CUSTOM_TARGET(${_uniqueTargetName} ALL DEPENDS ${${outfiles}})
ENDMACRO(PYQT4_WRAP_QRC)
