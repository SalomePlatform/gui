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
// TableViewer_ViewModel.cxx: implementation of the TableViewer_ViewModel class.

#include "TableViewer_ViewModel.h"
#include "TableViewer_ViewWindow.h"
#include "TableViewer_ViewManager.h"

#include <QtxTable.h>
/*!
  Constructor
*/
TableViewer_Viewer::TableViewer_Viewer()
:SUIT_ViewModel() 
{
}

/*!
  Destructor
*/
TableViewer_Viewer::~TableViewer_Viewer()
{
}

/*!
  Create new instance of view window on desktop \a theDesktop.
  \retval SUIT_ViewWindow* - created view window pointer.
*/
SUIT_ViewWindow* TableViewer_Viewer::createView( SUIT_Desktop* theDesktop )
{
  TableViewer_ViewWindow* vw = new TableViewer_ViewWindow( theDesktop, this );
  initView( vw );
  return vw;
}

/*!
  Start initialization of view window
  \param theVW - view window to be initialized
*/
void TableViewer_Viewer::initView( TableViewer_ViewWindow* theVW )
{
  theVW->initLayout();
  QtxTable* tbl = theVW->table();
  //if ( tbl && getViewManager() )
  //  tbl->viewport()->installEventFilter( getViewManager() );
}
