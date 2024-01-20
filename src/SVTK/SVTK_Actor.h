// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#ifndef SVTK_ACTOR_H
#define SVTK_ACTOR_H

#include "SVTK_Selector.h"
#include <TColStd_IndexedMapOfInteger.hxx>

class vtkRenderer;
class vtkShrinkFilter;
class vtkDataSetMapper;
class vtkUnstructuredGrid;

#include "SVTK.h"
#include "SVTK_DeviceActor.h"
#include "SVTK_Hash.h"
#include <vtkSmartPointer.h>

class SALOME_Actor;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

//! This class used for internal SVTK package purpose (highlight and prehighlight)
class SVTK_EXPORT SVTK_Actor : public SVTK_DeviceActor
{
public:
  static SVTK_Actor* New();

  vtkTypeMacro(SVTK_Actor,SVTK_DeviceActor)

  //! Initialiaze the instance completely
  virtual void
  Initialize();

  //! Allows to set an external source 
  void
  SetSource(vtkUnstructuredGrid* theUnstructuredGrid);

  //! Get its internal data set
  vtkUnstructuredGrid*
  GetSource();

  virtual void
  AddToRender(vtkRenderer* theRenderer);

  virtual void
  RemoveFromRender(vtkRenderer* theRenderer);


  //! Allow to recostruct selected cells from source SALOME_Actor and map of subindexes
  virtual void
  MapCells(SALOME_Actor* theMapActor, 
           const SVTK_TIndexedMapOfVtkId& theMapIndex);

  //! Allow to recostruct selected points from source SALOME_Actor and map of subindexes
  virtual void 
  MapPoints(SALOME_Actor* theMapActor, 
            const SVTK_TIndexedMapOfVtkId& theMapIndex);

  //! Allow to recostruct selected edges from source SALOME_Actor and map of subindexes
  virtual void 
  MapEdge(SALOME_Actor* theMapActor, 
          const SVTK_TIndexedMapOfVtkId& theMapIndex);

//! Allow to recostruct selected edges from source SALOME_Actor and map of subindexes using corner node ids
  virtual void 
  MapEdge(SALOME_Actor* theMapActor, 
          const SVTK_IndexedMapOfVtkIds& theMapCompositeIndex);
  

  const SVTK_TIndexedMapOfVtkId&
  GetMapIndex() const;

  const SVTK_IndexedMapOfVtkIds&
  GetMapCompositeIndex() const;

 protected:
  SVTK_TIndexedMapOfVtkId        myMapIndex;
  SVTK_IndexedMapOfVtkIds        myMapCompositeIndex;

  vtkSmartPointer<vtkUnstructuredGrid> myUnstructuredGrid;

  SVTK_Actor();
  virtual
  ~SVTK_Actor();
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
