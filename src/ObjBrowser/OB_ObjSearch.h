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
#ifndef OBJECT_BROWSER_OBJECT_SEARCH_HEADER
#define OBJECT_BROWSER_OBJECT_SEARCH_HEADER

#include "OB.h"

#include <qstring.h>

class OB_ListItem;
class OB_Browser;
class SUIT_DataObject;

class OB_EXPORT OB_ObjSearch
{
public:
  OB_ObjSearch( OB_Browser* );
  virtual ~OB_ObjSearch();

  void setPattern( const QString&, const bool, const bool );
  OB_Browser* browser() const;

  OB_ListItem* findFirst() const;
  OB_ListItem* findLast() const;
  OB_ListItem* findNext() const;
  OB_ListItem* findPrev() const;

protected:
  virtual OB_ListItem* current() const;
  virtual OB_ListItem* tail( const bool ) const;
  virtual OB_ListItem* next( OB_ListItem*, const bool ) const;
  virtual SUIT_DataObject* data( OB_ListItem* ) const;
  virtual bool matches( SUIT_DataObject* ) const;
  OB_ListItem* find( OB_ListItem*, const bool, const bool, const bool ) const;

private:
  OB_Browser* myBrowser;
  QString myData;
  bool myIsRegExp, myIsCaseSens;
};

#endif
