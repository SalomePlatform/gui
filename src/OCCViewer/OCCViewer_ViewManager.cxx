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
#include "OCCViewer_ViewManager.h"
#include "OCCViewer_ViewWindow.h"
#include "SUIT_Desktop.h"

/*!
  Constructor
*/
OCCViewer_ViewManager::OCCViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* theDesktop, bool DisplayTrihedron )
: SUIT_ViewManager( study, theDesktop, new OCCViewer_Viewer( DisplayTrihedron, false ) )
{
  setTitle( tr( "OCC_VIEW_TITLE" ) );
}

/*!
  Destructor
*/
OCCViewer_ViewManager::~OCCViewer_ViewManager()
{
}

/*!
  Fills popup menu with custom actions
 \param popup - popup menu to be filled with
*/
void OCCViewer_ViewManager::contextMenuPopup( QMenu* popup )
{
  SUIT_ViewManager::contextMenuPopup( popup );
  // if it is necessary invoke method CreatePopup of ViewPort
  // be sure that existing QPopupMenu menu is used for that.
}