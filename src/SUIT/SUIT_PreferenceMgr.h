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
// File:      SUIT_PreferenceMgr.h
// Author:    Sergey TELKOV

#ifndef SUIT_PREFERENCEMGR_H
#define SUIT_PREFERENCEMGR_H

// This class is obsoleted.
// It was created for backward compatibility with ResourceEdit from Salome version 3.x.x

#include "SUIT.h"

#include "QtxPagePrefMgr.h"

class SUIT_EXPORT SUIT_PreferenceMgr : public QtxPagePrefMgr
{
  Q_OBJECT

public:
  typedef enum { Auto, Space, Bool, Color, String, Selector,
                 DblSpin, IntSpin, Double, Integer,
                 GroupBox, Font, DirList, File } PrefItemType;

public:
  SUIT_PreferenceMgr( QtxResourceMgr*, QWidget* = 0 );
  virtual ~SUIT_PreferenceMgr();

  QVariant      itemProperty( const int, const QString& ) const;
  void          setItemProperty( const int, const QString&, const QVariant& );

  int           addItem( const QString&, const int pId = -1, const PrefItemType = Auto,
                         const QString& = QString(), const QString& = QString() );
};

#endif
