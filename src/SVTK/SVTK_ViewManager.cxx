// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "SVTK_ViewManager.h"
#include "SVTK_ViewModel.h"

#include <VTKViewer_ViewManager.h>

#include <SUIT_PreferenceMgr.h>
#include <SUIT_ResourceMgr.h>

/*!
  Constructor
*/
SVTK_ViewManager::SVTK_ViewManager( SUIT_Study* study, SUIT_Desktop* theDesktop ) 
: SUIT_ViewManager( study, theDesktop, new SVTK_Viewer() )
{
  setTitle( VTKViewer_ViewManager::tr( "VTK_VIEW_TITLE" ) );
}

/*!
  Destructor
*/
SVTK_ViewManager::~SVTK_ViewManager()
{
}

/*!
  \return corresponding main window
*/
SUIT_Desktop* SVTK_ViewManager::getDesktop()
{
  return myDesktop;
}

/*!
  Fills preference manager for viewer
*/
int SVTK_ViewManager::fillPreferences( SUIT_PreferenceMgr* thePrefMgr, const int theId )
{
  int aGrpId = thePrefMgr->addItem( tr( "PREF_GROUP_VTKVIEWER" ), theId,
                                    SUIT_PreferenceMgr::GroupBox );

  int vtkTS = thePrefMgr->addItem( tr( "PREF_TRIHEDRON_SIZE" ), aGrpId,
                                   SUIT_PreferenceMgr::DblSpin, "VTKViewer", "trihedron_size" );
  thePrefMgr->addItem( tr( "PREF_RELATIVE_SIZE" ), aGrpId, SUIT_PreferenceMgr::Bool,
                       "VTKViewer", "relative_size" );
  thePrefMgr->addItem( tr( "PREF_VIEWER_BACKGROUND" ), aGrpId,
                       SUIT_PreferenceMgr::Color, "VTKViewer", "background" );

  thePrefMgr->setItemProperty( "min", 1.0E-06, vtkTS );
  thePrefMgr->setItemProperty( "max", 150, vtkTS );

  return aGrpId;
}

/**
 * Fills values from resources
 */
void SVTK_ViewManager::fillFrom( SUIT_ResourceMgr* theMgr )
{
  SVTK_Viewer* aModel = dynamic_cast<SVTK_Viewer*>( getViewModel() );
  if ( !aModel )
    return;

  aModel->setBackgroundColor( theMgr->colorValue( "VTKViewer", "background",
                                                  aModel->backgroundColor() ) );
  aModel->setTrihedronSize(
     theMgr->doubleValue( "VTKViewer", "trihedron_size", aModel->trihedronSize() ),
     theMgr->booleanValue( "VTKViewer", "relative_size", aModel->trihedronRelative() ) );
}
