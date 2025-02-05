# -*- coding: iso-8859-1 -*-
# Copyright (C) 2010-2025  CEA, EDF, OPEN CASCADE
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
# Author : Guillaume Boulant (EDF)
# Example 1: creation of basic objects (O, Vx, Vy, Vz)
#

# Intialize the geompy factory with the active study
import salome

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

# Create the objects
Vx = geompy.MakeVectorDXDYDZ(10, 0, 0)
Vy = geompy.MakeVectorDXDYDZ(0, 10, 0)
Vz = geompy.MakeVectorDXDYDZ(0, 0, 10)
origin = geompy.MakeVertex(0, 0, 0)

# Register the objects in the active study
geompy.addToStudy( Vx, "Vx" )
geompy.addToStudy( Vy, "Vy" )
geompy.addToStudy( Vz, "Vz" )
geompy.addToStudy( origin, "origin" )


