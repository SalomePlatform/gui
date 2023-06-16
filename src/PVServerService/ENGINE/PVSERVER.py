# Copyright (C) 2015-2023  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Author : Adrien BRUNETON (CEA)
#

import PVSERVER_ORB__POA
import SALOME_ComponentPy
import SALOME_Embedded_NamingService_ClientPy
from PVSERVER_impl import PVSERVER_impl, MESSAGE 
    
class PVSERVER(PVSERVER_ORB__POA.PVSERVER_Gen,
              PVSERVER_impl):
    """
    Construct an instance of PVSERVER *service*.
    The class PVSERVER implements the CORBA interface PVSERVER_Gen (see PVSERVER_Gen.idl).
    It is NOT inherited from the classes SALOME_ComponentPy_i, as we present
    the class as a pure CORBA service, not as a standard SALOME component.
    """
    def __init__ ( self, orb, poa, contID, containerName, instanceName, 
                   interfaceName ):
        
        
        PVSERVER_impl.__init__(self)
        
        self._orb = orb
        self._poa = poa
        self._instanceName = instanceName
#         self._interfaceName = interfaceName
        self._containerName = containerName
#         self._contId = contID
        self._compo_o = None

        emb_ns = contID.get_embedded_NS_if_ssl()
        import CORBA
        if CORBA.is_nil(emb_ns):
            self._naming_service = SALOME_ComponentPy.SALOME_NamingServicePy_i( self._orb )
        else:
            self._naming_service = SALOME_Embedded_NamingService_ClientPy.SALOME_Embedded_NamingService_ClientPy(emb_ns)

        Component_path = self._containerName + "/" + self._instanceName
        MESSAGE(  'SALOME_ComponentPy_i Register' + str( Component_path ) )
        id_o = poa.activate_object(self)
        self._compo_o = poa.id_to_reference(id_o)
        
        ## And launch registration
        ##
        self._naming_service.Register(self._compo_o, Component_path)

    def getCorbaRef(self):
        return self._compo_o

    """ Override base class destroy to make sure we try to kill the pvserver
        before leaving.
    """
    def destroy(self):
        # TODO
        self.StopPVServer()

  