# -*- coding: iso-8859-1 -*-
import salome_pluginsmanager

DEMO_IS_ACTIVATED=True

# -------------------------------------------------------------------------
# Example 1: creation of basic objects.
# The plugin function is implemented here and declared in the pluginsmanager.
#
import salome

def trihedron(context):
    import geompy

    # Intialize the geompy factory with the active study
    activeStudy = context.study
    geompy.init_geom(activeStudy)

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

if DEMO_IS_ACTIVATED:
    salome_pluginsmanager.AddFunction('O,Vx,Vy,Vz',
                                      'Creates the trihedron',
                                      trihedron)


# -------------------------------------------------------------------------
# Example 1 bis: creation of basic objects and automatic display
def trihedron_withdisplay(context):
    import geompy

    # Intialize the geompy factory with the active study
    activeStudy = context.study
    geompy.init_geom(activeStudy)

    # Create the objects
    Vx = geompy.MakeVectorDXDYDZ(10, 0, 0)
    Vy = geompy.MakeVectorDXDYDZ(0, 10, 0)
    Vz = geompy.MakeVectorDXDYDZ(0, 0, 10)
    origin = geompy.MakeVertex(0, 0, 0)

    # Register the objects in the active study
    entries=[]
    entries.append(geompy.addToStudy( Vx, "Vx" ))
    entries.append(geompy.addToStudy( Vy, "Vy" ))
    entries.append(geompy.addToStudy( Vz, "Vz" ))
    entries.append(geompy.addToStudy( origin, "origin" ))

    # This part is to automatically display the objects in the active viewer.
    gcomp = salome.ImportComponentGUI("GEOM")
    for entry in entries:
        gcomp.createAndDisplayFitAllGO(entry)

if DEMO_IS_ACTIVATED:
    salome_pluginsmanager.AddFunction('O,Vx,Vy,Vz (2)',
                                      'Creates Basic GEOM objects',
                                      trihedron_withdisplay)

# -------------------------------------------------------------------------
# Example 2: creation of a shape with parameters that can be read from a GUI.
# The plugin function (tube_shapewithgui) delegates some action to
# dedicated imported modules (tube.py and tubedialog.py).
#
import tube

# A single dialog box is defined and recycled for every call. The
# fields are initialized with default values given by the tube factory
# tube.py.
import tubedialog
dialog = tubedialog.TubeDialog()
dialog.setData(tube.DEFAULT_RADIUS, tube.DEFAULT_LENGTH, tube.DEFAULT_WIDTH)

def tube_shapewithgui(context):
    global tube, dialog
    activeStudy = context.study

    # Get the parameter values from a gui dialog box. If the dialog is
    # closed using the Ok button, then the data are requested from the
    # gui and used to create the shape of the tube.
    dialog.exec_()
    if dialog.wasOk():
        radius, length, width = dialog.getData()
        shape = tube.createGeometry(activeStudy, radius, length, width)

if DEMO_IS_ACTIVATED:
    salome_pluginsmanager.AddFunction('Tube shape from parameters',
                                      'Creates a tube object from specified parameters',
                                      tube_shapewithgui)


# -------------------------------------------------------------------------
# Example 2 bis: creation of a mesh with parameters that can be read from a GUI.
# The plugin function (tube_meshwithgui) delegates some action to
# dedicated imported modules (tube.py and tubedialog.py).
#
def tube_meshwithgui(context):
    global tube, dialog
    activeStudy = context.study

    # Get the parameter values from a gui dialog box. If the dialog is
    # closed using the Ok button, then the data are requested from the
    # gui and used to create the shape of the tube.
    dialog.exec_()
    if dialog.wasOk():
        radius, length, width = dialog.getData()
        mesh = tube.createModel(activeStudy, radius, length, width)

if DEMO_IS_ACTIVATED:
    salome_pluginsmanager.AddFunction('Tube mesh from parameters',
                                      'Creates a tube object from specified parameters',
                                      tube_meshwithgui)

# -------------------------------------------------------------------------
# Example 3: run a shell session in a xterm to get a SALOME python console
def runSalomeShellSession(context):
    import os
    command="(xterm -e "+os.environ['KERNEL_ROOT_DIR']+"/runSession)&"
    os.system(command)

if DEMO_IS_ACTIVATED:
    salome_pluginsmanager.AddFunction('SALOME shell session',
                                      'Execute a SALOME shell session in an external xterm',
                                      runSalomeShellSession)
