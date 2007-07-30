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
// File   : LightApp_Browser.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
// 

#include "LightApp_Browser.h"
#include <SUIT_DataObject.h>
#include <SUIT_TreeModel.h>
#include <QtxTreeView.h>

/*!
  \class LightApp_Browser
  \brief Object browser customization.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
LightApp_Browser::LightApp_Browser( QWidget* parent )
: OB_Browser( parent )
{
  setModel( new SUIT_ProxyModel( this ) );
}

/*!
  \brief Constructor.
  \param root root data object
  \param parent parent widget
*/
LightApp_Browser::LightApp_Browser( SUIT_DataObject* root, QWidget* parent )
: OB_Browser( parent )
{
  setModel( new SUIT_ProxyModel( root, this ) );
  connect( treeView(), SIGNAL( sortingEnabled(bool ) ), 
	   model(), SLOT( setSortingEnabled( bool ) ) );
}

/*!
  \brief Get root object.
  \return root object
*/
SUIT_DataObject* LightApp_Browser::root() const
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  return m ? m->root() : 0;
}

/*!
  \brief Set root object.
  \param r new root object
*/
void LightApp_Browser::setRoot( SUIT_DataObject* r )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  if ( m ) 
    m->setRoot( r );
}

/*!
  \brief Get 'auto-update tree' flag value.
  \return 'auto-update tree' flag value
  \sa setAutoUpdate(), updateTree()
*/
bool LightApp_Browser::autoUpdate() const
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  return m ? m->autoUpdate() : false;
}

/*!
  \brief Set 'auto-update tree' flag value.

  If this flag is set to \c true (by default), the object browser is updated
  automatically when data tree is changed.

  \param on 'auto-update tree' flag value
  \sa autoUpdate(), updateTree()
*/
void LightApp_Browser::setAutoUpdate( const bool on )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  if ( m ) 
    m->setAutoUpdate( on );
}

/*!
  \brief Update object browser starting from the object \obj;
  open all branches automatically if \a autoOpen is \c true.
  \param obj starting object for updating
  \param autoOpen if \c true automatically open branches
*/
void LightApp_Browser::updateTree( SUIT_DataObject* obj, const bool autoOpen )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  if ( m ) {
    m->updateTree( obj );
    openLevels();
  }
}
