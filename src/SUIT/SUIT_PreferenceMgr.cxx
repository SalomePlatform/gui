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
// File:      SUIT_PreferenceMgr.cxx
// Author:    Sergey TELKOV

#include "SUIT_PreferenceMgr.h"

SUIT_PreferenceMgr::SUIT_PreferenceMgr( QtxResourceMgr* resMgr, QWidget* parent )
: QtxPagePrefMgr( resMgr, parent )
{
}

SUIT_PreferenceMgr::~SUIT_PreferenceMgr()
{
}

QVariant SUIT_PreferenceMgr::itemProperty( const int id, const QString& prop ) const
{
  QtxPreferenceItem* item = findItem( id, true );
  return item ? item->option( prop ) : QVariant();
}

void SUIT_PreferenceMgr::setItemProperty( const int id, const QString& prop, const QVariant& val )
{
  QtxPreferenceItem* item = findItem( id, true );
  if ( item )
    item->setOption( prop, val );
}

int SUIT_PreferenceMgr::addItem( const QString& title, const int pId,
                                 const SUIT_PreferenceMgr::PrefItemType type,
                                 const QString& sect, const QString& param )
{
  QtxPreferenceItem* item = findItem( title, true );
  if ( item )
    return item->id();

  QtxPreferenceItem* parent = 0;
  if ( pId == -1 )
  {
    QList<QtxPreferenceItem*> lst = childItems();
    for ( QList<QtxPreferenceItem*>::const_iterator it = lst.begin(); it != lst.end() && !parent; ++it )
      parent = *it;
  }
  else
    parent = findItem( pId );

  if ( !parent )
    parent = new QtxPagePrefListItem( QString(), this );

  switch( type )
  {
  case Auto:
    switch ( parent->depth() )
    {
    case 1:
      item = new QtxPagePrefTabsItem( title, parent, sect, param );
      break;
    case 2:
      item = new QtxPagePrefFrameItem( title, parent, sect, param );
      break;
    case 3:
      item = new QtxPagePrefGroupItem( title, parent, sect, param );
      break;
    }
    break;
  case Space:
    item = new QtxPagePrefSpaceItem( parent );
    break;
  case Bool:
    item = new QtxPagePrefCheckItem( title, parent, sect, param );
    break;
  case Color:
    item = new QtxPagePrefColorItem( title, parent, sect, param );
    break;
  case String:
    item = new QtxPagePrefEditItem( QtxPagePrefEditItem::String, title, parent, sect, param );
    break;
  case Selector:
    item = new QtxPagePrefSelectItem( title, parent, sect, param );
    break;
  case DblSpin:
    item = new QtxPagePrefSpinItem( QtxPagePrefSpinItem::Double, title, parent, sect, param );
    break;
  case IntSpin:
    item = new QtxPagePrefSpinItem( QtxPagePrefSpinItem::Integer, title, parent, sect, param );
    break;
  case Double:
    item = new QtxPagePrefEditItem( QtxPagePrefEditItem::Double, title, parent, sect, param );
    break;
  case Integer:
    item = new QtxPagePrefEditItem( QtxPagePrefEditItem::Integer, title, parent, sect, param );
    break;
  case GroupBox:
    item = new QtxPagePrefGroupItem( title, parent, sect, param );
    break;
  case Font:
    item = new QtxPagePrefFontItem( title, parent, sect, param );
    break;
  case File:
    item = new QtxPagePrefPathItem( QtxPagePrefPathItem::OpenFile, title, parent, sect, param );
    break;
  case DirList:
    item = new QtxPagePrefPathsItem( QtxPagePrefPathsItem::Directory, title, parent, sect, param );
    break;
  }

  return item ? item->id() : -1;
}
