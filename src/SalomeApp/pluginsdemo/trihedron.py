# -*- coding: iso-8859-1 -*-
# Example 1: creation of basic objects (O, Vx, Vy, Vz)
#

# Intialize the geompy factory with the active study
import geompy
import salome
geompy.init_geom(salome.myStudy)

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


