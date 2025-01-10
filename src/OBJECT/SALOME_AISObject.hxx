// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef SALOME_AISOBJECT_HXX
#define SALOME_AISOBJECT_HXX

#include <AIS_InteractiveObject.hxx>
#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

#include "SALOME_InteractiveObject.hxx"

class SALOME_AISObject : public AIS_InteractiveObject
{
public:
  Standard_EXPORT SALOME_AISObject();
  Standard_EXPORT ~SALOME_AISObject();

  Standard_EXPORT virtual Handle(SALOME_InteractiveObject) getIO() = 0;
  Standard_EXPORT virtual Standard_Boolean hasIO() = 0;

  Standard_EXPORT virtual Standard_CString getName() = 0;
  Standard_EXPORT virtual void setName(const Standard_CString aName) = 0;

public:
  DEFINE_STANDARD_RTTIEXT(SALOME_AISObject, AIS_InteractiveObject)
};

DEFINE_STANDARD_HANDLE(SALOME_AISObject, AIS_InteractiveObject)

#endif // SALOME_AISOBJECT_HXX
