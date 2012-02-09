# -*- coding: iso-8859-1 -*-

import salome
from salome.geom import geomtools

def addToStudy(study,shape,name):
    '''
    Helper function to add an object in the study. It returns the
    associated entry so that this study could be manipulated using
    study object functions.  
    '''
    studyId = study._get_StudyId()
    geompy = geomtools.getGeompy(studyId)
    entry = geompy.addToStudy( shape, "Tube" )
    return entry

# Available in SALOME 6.5 only
def displayShape_version65(shapeStudyEntry):
    gst = geomtools.GeomStudyTools()
    gst.displayShapeByEntry(shapeStudyEntry)

def eraseShape_version65(shapeStudyEntry):
    gst = geomtools.GeomStudyTools()
    gst.eraseShapeByEntry(shapeStudyEntry)

ModeShading = 1
DisplayMode=ModeShading
def displayShape(shapeStudyEntry):
    """This displays the shape specified by its entry in the study"""
    geomgui = salome.ImportComponentGUI("GEOM")            
    geomgui.createAndDisplayFitAllGO(shapeStudyEntry)
    geomgui.setDisplayMode(shapeStudyEntry, DisplayMode)

def eraseShape(shapeStudyEntry):
    """
    This erases from the viewers the shape specified by its entry
    in the study.
    """
    geomgui = salome.ImportComponentGUI("GEOM")
    eraseFromAllWindows=True
    geomgui.eraseGO(shapeStudyEntry,eraseFromAllWindows)
