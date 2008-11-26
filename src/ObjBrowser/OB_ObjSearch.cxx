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
#include <OB_ObjSearch.h>
#include <OB_ListItem.h>
#include <OB_Browser.h>

#include <qregexp.h>

OB_ObjSearch::OB_ObjSearch( OB_Browser* b )
: myBrowser( b )
{
}

OB_ObjSearch::~OB_ObjSearch()
{
}

OB_ListItem* OB_ObjSearch::tail( const bool first ) const
{
  QListView* lv = myBrowser->listView();
  return dynamic_cast<OB_ListItem*>( first ? lv->firstChild() : lv->lastItem() );
}

OB_ListItem* OB_ObjSearch::next( OB_ListItem* i, const bool forward ) const
{
  QListViewItemIterator it( i );
  if( forward )
    it++;
  else
    it--;
  return dynamic_cast<OB_ListItem*>( *it );
}

SUIT_DataObject* OB_ObjSearch::data( OB_ListItem* i ) const
{
  return i ? i->dataObject() : 0;
}

void OB_ObjSearch::setPattern( const QString& data, const bool is_reg_exp, const bool is_case_sens )
{
  myData = data;
  myIsRegExp = is_reg_exp;
  myIsCaseSens = is_case_sens;
}

bool OB_ObjSearch::matches( SUIT_DataObject* obj ) const
{
  if( !obj )
    return false;

  QString txt = obj->name();
  if( myIsRegExp )
  {
    QRegExp re( myData );
    re.setCaseSensitive( myIsCaseSens );
    return txt.contains( re );
  }
  else
    return txt.contains( myData, myIsCaseSens );
}

OB_Browser* OB_ObjSearch::browser() const
{
  return myBrowser;
}

OB_ListItem* OB_ObjSearch::findFirst() const
{
  return find( tail( true ), true, false, false );
}

OB_ListItem* OB_ObjSearch::findLast() const
{
  return find( tail( false ), false, false, false );
}

OB_ListItem* OB_ObjSearch::findNext() const
{
  return find( current(), true, true, true );
}

OB_ListItem* OB_ObjSearch::findPrev() const
{
  return find( current(), false, true, true );
}

OB_ListItem* OB_ObjSearch::find( OB_ListItem* i, const bool forward,
                                 const bool ignore_first, const bool cyclic ) const
{
  if( !i )
    i = tail( forward );

  if( ignore_first )
    i = next( i, forward );

  while( i && !matches( data( i ) ) )
    i = next( i, forward );

  if( !i && cyclic )
    return find( tail( forward ), forward, false, false );

  return i;
}

OB_ListItem* OB_ObjSearch::current() const
{
  return dynamic_cast<OB_ListItem*>( myBrowser->listView()->currentItem() );
}
