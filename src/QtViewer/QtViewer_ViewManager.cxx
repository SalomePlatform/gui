// Copyright (C) 2013-2023  CEA, EDF, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

#include "QtViewer_ViewManager.h"
#include "QtViewer_Viewer.h"

//=======================================================================
// Name    : QtViewer_ViewManager
// Purpose : Constructor
//=======================================================================
QtViewer_ViewManager::QtViewer_ViewManager( SUIT_Study* theStudy,
                                                    SUIT_Desktop* theDesktop,
                                                    SUIT_ViewModel* theModel )
: SUIT_ViewManager( theStudy, theDesktop, 
    theModel ? theModel : new QtViewer_Viewer("QtViewer") )
{
  setTitle( tr( "QTVIEWER_TITLE" ) );
}

//=======================================================================
// Name    : QtViewer_ViewManager
// Purpose : Destructor
//=======================================================================
QtViewer_ViewManager::~QtViewer_ViewManager()
{
}

//================================================================
// Function : getViewer
// Purpose  : 
//================================================================
QtViewer_Viewer* QtViewer_ViewManager::getViewer()
{
  return dynamic_cast<QtViewer_Viewer*>( getViewModel() );
}
