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
// File:      QtxActionMgrId.cxx
// Author:    Sergey TELKOV

#include "QtxActionMgrId.h"

QtxActionMgrId::QtxActionMgrId()
  : myId( -1 )
{
}

QtxActionMgrId::QtxActionMgrId( const int id )
  : myId( id )
{
}

QtxActionMgrId::QtxActionMgrId( const int id, const QString& ctx )
  : myId( id ),
    myCtx( ctx )
{
}

QtxActionMgrId::QtxActionMgrId( const QtxActionMgrId& other )
  : myId( other.myId ),
    myCtx( other.myCtx )
{
}

bool QtxActionMgrId::isNull() const
{
  return myId == -1;
}

int QtxActionMgrId::id() const
{
  return myId;
}

QString QtxActionMgrId::context() const
{
  return myCtx;
}

bool QtxActionMgrId::operator==( const QtxActionMgrId& other ) const
{
  return other.myId == myId && other.myCtx == myCtx;
}

bool QtxActionMgrId::operator!=( const QtxActionMgrId& other ) const
{
  return !( *this == other );
}

bool QtxActionMgrId::operator<( const QtxActionMgrId& other ) const
{
  if ( myId < other.myId )
    return true;
  else if ( myId == other.myId )
    return myCtx < other.myCtx;
  else
    return false;
}

QtxActionMgrId::operator int() const
{
  return myId;
}

QtxActionMgrId::operator QString() const
{
  return myCtx;
}
