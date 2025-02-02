# Copyright (C) 2007-2024  CEA, EDF
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
# Author: Adrien Bruneton
#

# CURVEPLOT detection for Salome - this is typically called by dependent modules
#
# The detection is simpler than for other prerequisites.
# See explanation in FindSalomeKERNEL.cmake.
#

IF(NOT SalomeCURVEPLOT_FIND_QUIETLY)
  MESSAGE(STATUS "Looking for Salome CURVEPLOT ...")
ENDIF()

SET(CMAKE_PREFIX_PATH "${CURVEPLOT_ROOT_DIR}")
SALOME_FIND_PACKAGE(SalomeCURVEPLOT SalomeCURVEPLOT CONFIG)

IF(NOT SalomeCURVEPLOT_FIND_QUIETLY)
  MESSAGE(STATUS "Found Salome CURVEPLOT: ${CURVEPLOT_ROOT_DIR}")
ENDIF()

