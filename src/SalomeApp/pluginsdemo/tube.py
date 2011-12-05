# -*- coding: iso-8859-1 -*-

import salome

DEFAULT_RADIUS = 100
DEFAULT_LENGTH = 300
DEFAULT_WIDTH  = 20

def createGeometry(study, radius=DEFAULT_RADIUS, length=DEFAULT_LENGTH, width=DEFAULT_WIDTH):
    '''
    This function creates the geometry on the specified study and with
    given parameters.
    '''
    print "TUBE: creating the geometry ..."
    import geompy
    geompy.init_geom(study)

    radius_ext = radius
    radius_int = radius_ext - width

    CylinderExt = geompy.MakeCylinderRH(radius_ext, length)
    CylinderInt = geompy.MakeCylinderRH(radius_int, length)
    Tube = geompy.MakeCut(CylinderExt, CylinderInt)
    entry = geompy.addToStudy( Tube, "Tube" )
    return Tube

def createGeometryWithPartition(study, radius=DEFAULT_RADIUS, length=DEFAULT_LENGTH, width=DEFAULT_WIDTH):
    '''
    This function create the geometrical shape with a partition so
    that the hexaedric algorithm could be used for meshing.
    '''
    import geompy
    shape = createGeometry(study,radius,length,width)

    # We have to create a partition so that we can use an hexaedric
    # meshing algorithm.
    print "TUBE: creating a partition ..."
    toolPlane = geompy.MakeFaceHW(2.1*length,2.1*radius,3)
    partition = geompy.MakePartition([shape], [toolPlane], [], [], geompy.ShapeType["SOLID"], 0, [], 0)
    entry = geompy.addToStudy( partition, "TubeWithPartition" )
    return partition
    
def createMesh(study, shape):
    '''This function creates the mesh of the specified shape on the specified study'''
    print "TUBE: creating the mesh ..."
    import smesh

    smesh.SetCurrentStudy(study)
    mesh = smesh.Mesh(shape)
    Regular_1D = mesh.Segment()
    Nb_Segments = Regular_1D.NumberOfSegments(10)
    Nb_Segments.SetDistrType( 0 )
    Quadrangle_2D = mesh.Quadrangle()
    Hexa_3D = mesh.Hexahedron()

    isDone = mesh.Compute()

    if salome.sg.hasDesktop():
        smesh.SetName(mesh.GetMesh(), 'TubeMesh')
        smesh.SetName(Regular_1D.GetAlgorithm(), 'Regular_1D')
        smesh.SetName(Nb_Segments, 'Nb. Segments_1')
        smesh.SetName(Quadrangle_2D.GetAlgorithm(), 'Quadrangle_2D')
        smesh.SetName(Hexa_3D.GetAlgorithm(), 'Hexa_3D')
        salome.sg.updateObjBrowser(0)

    return mesh


def createModel(study, radius=DEFAULT_RADIUS, length=DEFAULT_LENGTH,width=DEFAULT_WIDTH):
    '''
    This function create the geomtrical shape AND the associated mesh.
    '''
    # We first create a shape with a partition so that the hexaedric
    # algorithm could be used.
    shape = createGeometryWithPartition(study,radius,length,width)

    # Then the mesh can be defined and computed
    mesh = createMesh(study,shape)
    
def exportModel(mesh, filename):
    '''
    This exports the mesh to the specified filename in the med format
    '''
    print "TUBE: exporting mesh to file %s ..."%filename
    import SMESH
    mesh.ExportMED(filename, 0, SMESH.MED_V2_2, 1 )


#
# ===================================================================
# Use cases and test functions
# ===================================================================
#
def TEST_createGeometry():
    salome.salome_init()
    theStudy=salome.myStudy
    createGeometry(theStudy)

def TEST_createMesh():
    salome.salome_init()
    theStudy=salome.myStudy
    shape = createGeometryWithPartition(theStudy)
    mesh  = createMesh(theStudy, shape)

def TEST_createModel():
    salome.salome_init()
    theStudy=salome.myStudy
    createModel(theStudy)

def TEST_exportModel():
    salome.salome_init()
    theStudy=salome.myStudy
    shape = createGeometryWithPartition(theStudy)
    mesh  = createMesh(theStudy, shape)
    exportModel(mesh,"tubemesh.med")
    
if __name__ == "__main__":
    #TEST_createGeometry()
    #TEST_createMesh()
    TEST_createModel()
    #TEST_exportModel()
