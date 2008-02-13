//  SALOME VTKViewer : build VTK viewer into Salome desktop
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$

#ifndef __SVTK_ACTOR2D_WIDGET__
#define __SVTK_ACTOR2D_WIDGET__

#include "SVTK.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include <vtkInteractorObserver.h>
#include <vtkCommand.h>

class vtkActor2D;
class vtkRenderer;


//----------------------------------------------------------------------------
class SVTK_EXPORT SVTK_Actor2DWidget : public vtkInteractorObserver
{
public:
  static SVTK_Actor2DWidget *New();
  vtkTypeMacro(SVTK_Actor2DWidget, vtkInteractorObserver);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetActor2D(vtkActor2D *theActor2D);
  vtkGetObjectMacro(Actor2D, vtkActor2D);
  
  // Description:
  // Methods for turning the interactor observer on and off.
  virtual void SetEnabled(int);

  //! connect to this event to process editing
  enum Event
  {
    EditEvent = vtkCommand::UserEvent + 1000
  };

protected:
  SVTK_Actor2DWidget();
  ~SVTK_Actor2DWidget();

  // the actor that is used
  vtkActor2D *Actor2D;

  //! handles the events
  static void ProcessEvents(vtkObject* object, 
                            unsigned long event,
                            void* clientdata, 
                            void* calldata);

  //! ProcessEvents() dispatches to these methods.
  void OnLeftButtonDown();
  void OnLeftButtonUp();
  void OnRightButtonDown();
  void OnRightButtonUp();
  void OnMouseMove();

  //! used to compute relative movements
  double StartPosition[2];
  
  //! manage the state of the widget
  int State;

  //! use this to track whether left/right button was pressed to gate action on button up event.
  int LeftButtonDown;
  int RightButtonDown;
  enum WidgetState
  {
    Moving=0,
    AdjustingP1,
    AdjustingP2,
    AdjustingP3,
    AdjustingP4,
    AdjustingE1,
    AdjustingE2,
    AdjustingE3,
    AdjustingE4,
    Inside,
    Outside
  };

  // use to determine what state the mouse is over, edge1 p1, etc.
  // returns a state from the WidgetState enum above
  int ComputeStateBasedOnPosition(int X, int Y, int *pos1, int *pos2);

  // set the cursor to the correct shape based on State argument
  void SetCursor(int State);

private:
  SVTK_Actor2DWidget(const SVTK_Actor2DWidget&);  //Not implemented
  void operator=(const SVTK_Actor2DWidget&);  //Not implemented
};


#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // __SVTK_ACTOR2D_WIDGET__
