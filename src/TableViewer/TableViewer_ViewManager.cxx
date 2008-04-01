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
#include "TableViewer_ViewManager.h"
#include "TableViewer_ViewModel.h"
#include "TableViewer_ViewWindow.h"

#include <SUIT_Study.h>
#include <SUIT_Desktop.h>
#include <SUIT_PreferenceMgr.h>
#include <SUIT_ResourceMgr.h>

/*!
  Constructor
*/
TableViewer_ViewManager::TableViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* desk ) 
: SUIT_ViewManager( study, desk, new TableViewer_Viewer() )
{
  setTitle( tr( "TABLE_VIEWER_VIEW_TITLE" ) );
}

/*!
  Destructor
*/
TableViewer_ViewManager::~TableViewer_ViewManager()
{
}

/*!
  \return corresponding viewer
*/
TableViewer_Viewer* TableViewer_ViewManager::getTableModel() const
{
  return (TableViewer_Viewer*)myViewModel;
}

/*!
  Adds new view
  \param theView - view to be added
*/
/*bool TableViewer_ViewManager::insertView( SUIT_ViewWindow* theView )
{
  bool res = SUIT_ViewManager::insertView( theView );
  if ( res )
  {
    TableViewer_ViewWindow* view = (TableViewer_ViewWindow*)theView;
    connect( view, SIGNAL( cloneView() ), this, SLOT( onCloneView() ) );
  }
  return res;
}
*/
/*!
  Creates new view
*/
void TableViewer_ViewManager::createView()
{
  createViewWindow();
}

/*!
  SLOT: called if action "Clone view" is activated, emits signal cloneView()
*/
/*void TableViewer_ViewManager::onCloneView()
{
  SUIT_ViewWindow* vw = createViewWindow();

  TableViewer_ViewWindow  *newWnd = 0, *clonedWnd = 0;
  if( vw && vw->inherits( "TableViewer_ViewWindow" ) )
    newWnd = ( TableViewer_ViewWindow* )vw;
  if( sender() && sender()->inherits( "TableViewer_ViewWindow" ) )
    clonedWnd = ( TableViewer_ViewWindow* )sender();
  
  if( newWnd && clonedWnd )
    emit cloneView( clonedWnd->getViewFrame(), newWnd->getViewFrame() );
}
*/

/*!
  Fills preference manager for viewer
*/
int TableViewer_ViewManager::fillPreferences( SUIT_PreferenceMgr* thePrefMgr, const int theId )
{
  int aGrpId = thePrefMgr->addItem( tr( "PREF_GROUP_TABLEVIEWER" ), theId,
                                        SUIT_PreferenceMgr::GroupBox );
  return aGrpId;
}

/**
 * Fills values from resources
 */
void TableViewer_ViewManager::fillFrom( SUIT_ResourceMgr* theMgr )
{

}
