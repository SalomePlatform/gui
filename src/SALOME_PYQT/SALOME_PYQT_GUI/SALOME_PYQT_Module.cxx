//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : SALOME_PYQT_Module.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "SALOME_PYQT_Module.h"
#include "SalomeApp_Application.h"
#include "SALOME_PYQT_ModuleLight.h"

#include <SALOME_LifeCycleCORBA.hxx>
#include <Container_init_python.hxx>
#include <CORBA.h>


//
// NB: Library initialization
// Since the SalomePyQtGUILight library is not imported in Python it's initialization function
// should be called manually (and only once) in order to initialize global sip data
// and to get C API from sip : sipBuildResult for example
//

#define INIT_FUNCTION initSalomePyQtGUILight
#if defined(SIP_STATIC_MODULE)
extern "C" void INIT_FUNCTION();
#else
PyMODINIT_FUNC INIT_FUNCTION();
#endif

/*!
  \fn CAM_Module* createModule()
  \brief Module factory function.
  \internal
  
  Creates an instance of SALOME_PYQT_Module object by request
  of an application when the module is loaded and initialized.

  \return new module object
*/

extern "C" {
  SALOME_PYQT_EXPORT CAM_Module* createModule() {

    static bool alreadyInitialized = false;
    if ( !alreadyInitialized ) {
      // call only once (see comment above) !

      PyEval_RestoreThread( KERNEL_PYTHON::_gtstate);
      INIT_FUNCTION();
      PyEval_ReleaseThread( KERNEL_PYTHON::_gtstate);
      alreadyInitialized = !alreadyInitialized;
    }
    return new SALOME_PYQT_Module();
  }
}


/*!
  \var __DEFAULT_NAME__ - Default name of the module, replaced at the moment of module creation
*/
const char* __DEFAULT_NAME__  = "SALOME_PYQT_Module";

/*!
 * Constructor
 */
SALOME_PYQT_Module::SALOME_PYQT_Module()
  : SalomeApp_Module(__DEFAULT_NAME__),
    LightApp_Module(__DEFAULT_NAME__),
    SALOME_PYQT_ModuleLight()
{
}

/*!
 * Destructor
 */
SALOME_PYQT_Module::~SALOME_PYQT_Module()
{
}

/*!
 * Get module engine, returns nil var if engine is not found in LifeCycleCORBA
 */
Engines::Component_var SALOME_PYQT_Module::getEngine() const
{
  Engines::Component_var comp;
  // temporary solution
  try {
    comp = getApp()->lcc()->FindOrLoad_Component( "FactoryServerPy", name().toLatin1() );
  }
  catch (CORBA::Exception&) {
  }
  return comp;
}

/*!
 * Get module engine IOR, returns empty string if engine is not found in LifeCycleCORBA
 */
QString SALOME_PYQT_Module::engineIOR() const
{
  if ( !CORBA::is_nil( getEngine() ) )
    return QString( getApp()->orb()->object_to_string( getEngine() ) );
  return QString( "" );
}

CAM_DataModel* SALOME_PYQT_Module::createDataModel()
{
  MESSAGE( "SALOME_PYQT_Module::createDataModel()" );
  CAM_DataModel * dm = SalomeApp_Module::createDataModel();
  return dm;
}

/*!
  \brief Process GUI action (from main menu, toolbar or 
  context popup menu action).
*/
void SALOME_PYQT_Module::onGUIEvent(){
  SALOME_PYQT_ModuleLight::onGUIEvent();
}

/*!
  \brief Signal handler closing(SUIT_ViewWindow*) of a view
  \param pview view being closed
*/
void SALOME_PYQT_Module::onViewClosed( SUIT_ViewWindow* pview )
{
  SALOME_PYQT_ModuleLight::onViewClosed( pview );
}

/*!
  \breif Process application preferences changing.

  Called when any application setting is changed.

  \param module preference module
  \param section preference resource file section
  \param setting preference resource name
*/
void SALOME_PYQT_ModuleLight::preferenceChanged( const QString& module, 
					    const QString& section, 
					    const QString& setting )
{
  SALOME_PYQT_ModuleLight::preferenceChanged(module,section,setting);
}

/*!
  \brief Signal handler windowActivated(SUIT_ViewWindow*) of SUIT_Desktop
  \param pview view being activated
*/
void SALOME_PYQT_Module::onActiveViewChanged( SUIT_ViewWindow* pview )
{
  SALOME_PYQT_ModuleLight::onActiveViewChanged(pview);
}

/*!
  \brief Signal handler cloneView() of OCCViewer_ViewWindow
  \param pview view being cloned
*/
void SALOME_PYQT_Module::onViewCloned( SUIT_ViewWindow* pview )
{
  SALOME_PYQT_ModuleLight::onViewCloned(pview);
}
