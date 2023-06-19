// Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : 
//  Author : Alexander SLADKOV

#ifndef SVTK_Event_h
#define SVTK_Event_h

#include <vtkCommand.h>

namespace SVTK
{
  enum Event 
  {
    SpaceMouseMoveEvent = vtkCommand::UserEvent + 1000, 
    SpaceMouseButtonEvent,
    PanLeftEvent,
    PanRightEvent,
    PanUpEvent,
    PanDownEvent,
    ZoomInEvent,
    ZoomOutEvent,
    RotateLeftEvent,
    RotateRightEvent,
    RotateUpEvent,
    RotateDownEvent,
    PlusSpeedIncrementEvent,
    MinusSpeedIncrementEvent,
    SetSpeedIncrementEvent,

    SetSMDecreaseSpeedEvent,
    SetSMIncreaseSpeedEvent,
    SetSMDominantCombinedSwitchEvent,

    StartZoom,
    StartPan,
    StartRotate,
    StartGlobalPan,
    StartFitArea,

    SetRotateGravity,
    SetRotateSelected,
    StartPointSelection,
    
    BBCenterChanged,
    RotationPointChanged,
    ChangeRotationPoint,

    SetFocalPointGravity,
    SetFocalPointSelected,
    StartFocalPointSelection,
    FocalPointChanged,

	  StartInteractiveSelection,
	  InteractiveSelectionChanged,
    InteractiveSelectionFinished,
        
    OperationFinished, // rnv: invoked when SpinXY, RotateXY, DollyXY or PanXY operation is finished.

    StopCurrentOperation,
    LastEvent
  };
}
  
#endif
