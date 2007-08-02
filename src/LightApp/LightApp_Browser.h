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
// File   : LightApp_Browser.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
// 

#ifndef LIGHTAPP_BROWSER_H
#define LIGHTAPP_BROWSER_H

#include "LightApp.h"
#include <OB_Browser.h>
#include <SUIT_DataObject.h>

class QShortcut;

class LIGHTAPP_EXPORT LightApp_Browser : public OB_Browser
{
  Q_OBJECT

public:
  LightApp_Browser( QWidget* = 0 );
  LightApp_Browser( SUIT_DataObject*, QWidget* = 0 );

  SUIT_DataObject* root() const;
  void             setRoot( SUIT_DataObject* );

  bool             autoUpdate() const;
  void             setAutoUpdate( const bool );

  void             updateTree( SUIT_DataObject* = 0, const bool = true );

  int              updateKey() const;
  void             setUpdateKey( const int );

  DataObjectList   getSelected() const;
  void             getSelected( DataObjectList& ) const;

  void             setSelected( const SUIT_DataObject*, const bool = false );
  void             setSelected( const DataObjectList&, const bool = false );

private:
  void             init( SUIT_DataObject* );

signals:
  void             requestUpdate();

private:
  QShortcut*       myShortcut;
};

#endif // LIGHTAPP_BROWSER_H
