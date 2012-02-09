# -*- coding: iso-8859-1 -*-
import salome_pluginsmanager

DEMO_IS_ACTIVATED=False

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
import myhelper

# A single dialog box is defined and recycled for every call. The
# fields are initialized with default values given by the tube factory
# tube.py.
import tubedialog
dialog = tubedialog.TubeDialog()
dialog.setData(tube.DEFAULT_RADIUS, tube.DEFAULT_LENGTH, tube.DEFAULT_WIDTH)

def tube_shapewithgui(context):
    global tube, myhelper, dialog
    activeStudy = context.study

    # Get the parameter values from a gui dialog box. If the dialog is
    # closed using the Ok button, then the data are requested from the
    # gui and used to create the shape of the tube.
    dialog.exec_()
    if dialog.wasOk():
        radius, length, width = dialog.getData()
        shape = tube.createGeometry(activeStudy, radius, length, width)
        entry = myhelper.addToStudy(activeStudy, shape, "Tube" )
        myhelper.displayShape(entry)

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
# Example 2 ter: creation of a geom object with a preview function in
# the dialog box. This use case is more complex from the gui point of
# view because the dialog box is a not modal so that we can have
# interaction with the complete SALOME application. This modal
# situation requires to connect button click signal on global
# functions named the "callback" functions.
#
import tubedialogWithApply
dialogWithApply = tubedialogWithApply.TubeDialogWithApply()
dialogWithApply.setData(tube.DEFAULT_RADIUS, tube.DEFAULT_LENGTH, tube.DEFAULT_WIDTH)
activeStudy = None
previewShapeEntry = None

def acceptCallback():
    """Action to do when click on Ok"""
    global tube, dialogWithApply, activeStudy, previewShapeEntry, deletePreviewShape
    dialogWithApply.accept()

    if previewShapeEntry is not None:
        deletePreviewShape()

    radius, length, width = dialogWithApply.getData()
    shape = tube.createGeometry(activeStudy, radius, length, width)
    entry = myhelper.addToStudy(activeStudy, shape, "Tube" )
    myhelper.displayShape(entry)
    

def rejectCallback():
    """Action to do when click on Cancel"""
    global tube, dialogWithApply, activeStudy, previewShapeEntry, deletePreviewShape
    dialogWithApply.reject()

    if previewShapeEntry is not None:
        deletePreviewShape()

def applyCallback():
    """Action to do when click on Apply"""
    global tube, dialogWithApply, activeStudy, previewShapeEntry, deletePreviewShape
    # We first have to destroy the currently displayed preview shape.
    if previewShapeEntry is not None:
        deletePreviewShape()

    # Then we can create the new shape with the new parameter values
    radius, length, width = dialogWithApply.getData()
    shape = tube.createGeometry(activeStudy, radius, length, width)
    previewShapeEntry = myhelper.addToStudy(activeStudy, shape, "Tube" )
    myhelper.displayShape(previewShapeEntry)

def deletePreviewShape():
    global activeStudy, previewShapeEntry

    from salome.kernel.studyedit import getStudyEditor, getStudyIdFromStudy
    from salome.kernel.services import IDToSObject, IDToObject

    # WARN: please be aware that to delete a geom object, you have
    # three operations to perform:
    # 1. erase the shape from the viewer
    # 2. delete the entry in the study
    # 3. destroy the geom object
    myhelper.eraseShape(previewShapeEntry)
    item = IDToSObject(previewShapeEntry)
    geomObj = IDToObject(previewShapeEntry)    
    ste = getStudyEditor(getStudyIdFromStudy(activeStudy))
    ste.removeItem(item,True)
    geomObj.Destroy()
    previewShapeEntry = None
    
dialogWithApply.handleAcceptWith(acceptCallback)
dialogWithApply.handleRejectWith(rejectCallback)
dialogWithApply.handleApplyWith(applyCallback)

def tube_shapewithguiAndPreview(context):
    global tube, dialogWithApply, activeStudy
    activeStudy = context.study
    dialogWithApply.open()

if DEMO_IS_ACTIVATED:
    salome_pluginsmanager.AddFunction('Tube geometry from parameters with preview',
                                      'Creates a tube object from specified parameters',
                                      tube_shapewithguiAndPreview)



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
