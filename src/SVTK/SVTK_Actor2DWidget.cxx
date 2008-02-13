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

#include "SVTK_Actor2DWidget.h"

#include <vtkActor2D.h>
#include <vtkCallbackCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCoordinate.h>

#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkCoordinate.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro(SVTK_Actor2DWidget);
vtkCxxSetObjectMacro(SVTK_Actor2DWidget, Actor2D, vtkActor2D);

SVTK_Actor2DWidget::SVTK_Actor2DWidget()
{
  this->Actor2D = NULL;
  this->EventCallbackCommand->SetCallback(SVTK_Actor2DWidget::ProcessEvents);
  this->State = SVTK_Actor2DWidget::Outside;
  this->LeftButtonDown = 0;
  this->RightButtonDown = 0;
  this->Priority = 0.55;
}

SVTK_Actor2DWidget::~SVTK_Actor2DWidget()
{
  this->SetActor2D(0);
}

void SVTK_Actor2DWidget::SetEnabled(int enabling)
{
  if ( ! this->Interactor )
    {
    vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
    return;
    }
  
  if ( enabling ) 
    {
    vtkDebugMacro(<<"Enabling line widget");
    if ( this->Enabled ) //already enabled, just return
      {
      return;
      }
    
    if ( ! this->CurrentRenderer )
      {
      this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(
        this->Interactor->GetLastEventPosition()[0],
        this->Interactor->GetLastEventPosition()[1]));
      if (this->CurrentRenderer == NULL)
        {
        return;
        }
      }

    this->Enabled = 1;
    
    // listen for the following events
    vtkRenderWindowInteractor *i = this->Interactor;
    i->AddObserver(vtkCommand::MouseMoveEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::LeftButtonPressEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::LeftButtonReleaseEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::RightButtonPressEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::RightButtonReleaseEvent, 
                   this->EventCallbackCommand, this->Priority);

    // Add the scalar bar
    this->CurrentRenderer->AddViewProp(this->Actor2D);
    this->InvokeEvent(vtkCommand::EnableEvent,NULL);
    }
  else //disabling------------------------------------------
    {
    vtkDebugMacro(<<"Disabling line widget");
    if ( ! this->Enabled ) //already disabled, just return
      {
      return;
      }
    this->Enabled = 0;

    // don't listen for events any more
    this->Interactor->RemoveObserver(this->EventCallbackCommand);

    // turn off the line
    this->CurrentRenderer->RemoveActor(this->Actor2D);
    this->InvokeEvent(vtkCommand::DisableEvent,NULL);
    this->SetCurrentRenderer(NULL);
    }
  
  this->Interactor->Render();
}

void SVTK_Actor2DWidget::ProcessEvents(vtkObject* vtkNotUsed(object), 
                                       unsigned long event,
                                       void* clientdata, 
                                       void* vtkNotUsed(calldata))
{
  SVTK_Actor2DWidget* self = reinterpret_cast<SVTK_Actor2DWidget *>( clientdata );
  
  //okay, let's do the right thing
  switch(event)
    {
    case vtkCommand::LeftButtonPressEvent:
      self->OnLeftButtonDown();
      break;
    case vtkCommand::LeftButtonReleaseEvent:
      self->OnLeftButtonUp();
      break;
    case vtkCommand::RightButtonPressEvent:
      self->OnRightButtonDown();
      break;
    case vtkCommand::RightButtonReleaseEvent:
      self->OnRightButtonUp();
      break;
    case vtkCommand::MouseMoveEvent:
      self->OnMouseMove();
      break;
    }
}


int SVTK_Actor2DWidget::ComputeStateBasedOnPosition(int X, int Y, 
                                                    int *pos1, int *pos2)
{
  int Result;
  
  // what are we modifying? The position, or size?
  // if size what piece?
  // if we are within 7 pixels of an edge...
  int e1 = 0;
  int e2 = 0;
  int e3 = 0;
  int e4 = 0;
  if (X - pos1[0] < 7)
    {
    e1 = 1;
    }
  if (pos2[0] - X < 7)
    {
    e3 = 1;
    }
  if (Y - pos1[1] < 7)
    {
    e2 = 1;
    }
  if (pos2[1] - Y < 7)
    {
    e4 = 1;
    }

  // assume we are moving
  Result = SVTK_Actor2DWidget::Moving;
  // unless we are on a corner or edges
  if (e2)
    {
    Result = SVTK_Actor2DWidget::AdjustingE2;
    }
  if (e4)
    {
    Result = SVTK_Actor2DWidget::AdjustingE4;
    }
  if (e1)
    {
    Result = SVTK_Actor2DWidget::AdjustingE1;
    if (e2)
      {
      Result = SVTK_Actor2DWidget::AdjustingP1;
      }
    if (e4)
      {
      Result = SVTK_Actor2DWidget::AdjustingP4;
      }
    }
  if (e3)
    {
    Result = SVTK_Actor2DWidget::AdjustingE3;
    if (e2)
      {
      Result = SVTK_Actor2DWidget::AdjustingP2;
      }
    if (e4)
      {
      Result = SVTK_Actor2DWidget::AdjustingP3;
      }
    }

  return Result;
}

void SVTK_Actor2DWidget::SetCursor(int cState)
{
  switch (cState)
    {
    case SVTK_Actor2DWidget::AdjustingP1:
      this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_SIZESW);
      break;
    case SVTK_Actor2DWidget::AdjustingP3:
      this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_SIZENE);
      break;
    case SVTK_Actor2DWidget::AdjustingP2:
      this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_SIZESE);
      break;
    case SVTK_Actor2DWidget::AdjustingP4:
      this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_SIZENW);
      break;
    case SVTK_Actor2DWidget::AdjustingE1:
    case SVTK_Actor2DWidget::AdjustingE3:
      this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_SIZEWE);
      break;
    case SVTK_Actor2DWidget::AdjustingE2:
    case SVTK_Actor2DWidget::AdjustingE4:
      this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_SIZENS);
      break;
    case SVTK_Actor2DWidget::Moving:
      this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_SIZEALL);
      break;        
    }
}


void SVTK_Actor2DWidget::OnLeftButtonDown()
{
  if(!this->Actor2D)
    return;

  // We're only here is we are enabled
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // are we over the widget?
  //this->Interactor->FindPokedRenderer(X,Y);
  int *pos1 = this->Actor2D->GetPositionCoordinate()
    ->GetComputedDisplayValue(this->CurrentRenderer);
  int *pos2 = this->Actor2D->GetPosition2Coordinate()
    ->GetComputedDisplayValue(this->CurrentRenderer);

  // are we not over the scalar bar, ignore
  if (X < pos1[0] || X > pos2[0] || Y < pos1[1] || Y > pos2[1])
    {
    return;
    }
  
  if(this->Interactor->GetRepeatCount() > 1){
    this->InvokeEvent(SVTK_Actor2DWidget::EditEvent, NULL);
    return;
  }

  // start a drag, store the normalized view coords
  double X2 = X;
  double Y2 = Y;
  // convert to normalized viewport coordinates
  this->CurrentRenderer->DisplayToNormalizedDisplay(X2,Y2);
  this->CurrentRenderer->NormalizedDisplayToViewport(X2,Y2);
  this->CurrentRenderer->ViewportToNormalizedViewport(X2,Y2);
  this->StartPosition[0] = X2;
  this->StartPosition[1] = Y2;

  this->State = this->ComputeStateBasedOnPosition(X, Y, pos1, pos2);
  this->SetCursor(this->State);
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  this->LeftButtonDown = 1;
}

void SVTK_Actor2DWidget::OnMouseMove()
{
  if(!this->Actor2D)
    return;

  // compute some info we need for all cases
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  
  // compute the display bounds of the scalar bar if we are inside or outside
  int *pos1, *pos2;
  if (this->State == SVTK_Actor2DWidget::Outside ||
      this->State == SVTK_Actor2DWidget::Inside)
    {
    pos1 = this->Actor2D->GetPositionCoordinate()
      ->GetComputedDisplayValue(this->CurrentRenderer);
    pos2 = this->Actor2D->GetPosition2Coordinate()
      ->GetComputedDisplayValue(this->CurrentRenderer);
  
    if (this->State == SVTK_Actor2DWidget::Outside)
      {
      // if we are not over the scalar bar, ignore
      if (X < pos1[0] || X > pos2[0] ||
          Y < pos1[1] || Y > pos2[1])
        {
        return;
        }
      // otherwise change our state to inside
      this->State = SVTK_Actor2DWidget::Inside;
      }
  
    // if inside, set the cursor to the correct shape
    if (this->State == SVTK_Actor2DWidget::Inside)
      {
      // if we have left then change cursor back to default
      if (X < pos1[0] || X > pos2[0] ||
          Y < pos1[1] || Y > pos2[1])
        {
        this->State = SVTK_Actor2DWidget::Outside;
        this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_DEFAULT);
        return;
        }
      // adjust the cursor based on our position
      this->SetCursor(this->ComputeStateBasedOnPosition(X,Y,pos1,pos2));
      return;
      }
    }
  
  double XF = X;
  double YF = Y;
  // convert to normalized viewport coordinates
  this->CurrentRenderer->DisplayToNormalizedDisplay(XF,YF);
  this->CurrentRenderer->NormalizedDisplayToViewport(XF,YF);
  this->CurrentRenderer->ViewportToNormalizedViewport(XF,YF);
  
  // there are four parameters that can be adjusted
  double *fpos1 = this->Actor2D->GetPositionCoordinate()->GetValue();
  double *fpos2 = this->Actor2D->GetPosition2Coordinate()->GetValue();
  double par1[2];
  double par2[2];
  par1[0] = fpos1[0];
  par1[1] = fpos1[1];
  par2[0] = fpos1[0] + fpos2[0];  
  par2[1] = fpos1[1] + fpos2[1];  
    
  // based on the state, adjust the ScalarBar parameters
  switch (this->State)
    {
    case SVTK_Actor2DWidget::AdjustingP1:
      par1[0] = par1[0] + XF - this->StartPosition[0];
      par1[1] = par1[1] + YF - this->StartPosition[1];
      break;
    case SVTK_Actor2DWidget::AdjustingP2:
      par2[0] = par2[0] + XF - this->StartPosition[0];
      par1[1] = par1[1] + YF - this->StartPosition[1];
      break;
    case SVTK_Actor2DWidget::AdjustingP3:
      par2[0] = par2[0] + XF - this->StartPosition[0];
      par2[1] = par2[1] + YF - this->StartPosition[1];
      break;
    case SVTK_Actor2DWidget::AdjustingP4:
      par1[0] = par1[0] + XF - this->StartPosition[0];
      par2[1] = par2[1] + YF - this->StartPosition[1];
      break;
    case SVTK_Actor2DWidget::AdjustingE1:
      par1[0] = par1[0] + XF - this->StartPosition[0];
      break;
    case SVTK_Actor2DWidget::AdjustingE2:
      par1[1] = par1[1] + YF - this->StartPosition[1];
      break;
    case SVTK_Actor2DWidget::AdjustingE3:
      par2[0] = par2[0] + XF - this->StartPosition[0];
      break;
    case SVTK_Actor2DWidget::AdjustingE4:
      par2[1] = par2[1] + YF - this->StartPosition[1];
      break;
    case SVTK_Actor2DWidget::Moving:
      // first apply the move
      par1[0] = par1[0] + XF - this->StartPosition[0];
      par1[1] = par1[1] + YF - this->StartPosition[1];
      par2[0] = par2[0] + XF - this->StartPosition[0];
      par2[1] = par2[1] + YF - this->StartPosition[1];
      break;
    }
  
  // push the change out to the scalar bar
  // make sure the scalar bar doesn't shrink to nothing
  if (par2[0] > par1[0] && par2[1] > par1[1])
    {
    this->Actor2D->GetPositionCoordinate()->SetValue(par1[0],par1[1]);
    this->Actor2D->GetPosition2Coordinate()->
      SetValue(par2[0] - par1[0], par2[1] - par1[1]);
    this->StartPosition[0] = XF;
    this->StartPosition[1] = YF;      
    }
  
  // start a drag
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent,NULL);
  this->Interactor->Render();
}

void SVTK_Actor2DWidget::OnLeftButtonUp()
{
  if (this->State == SVTK_Actor2DWidget::Outside || this->LeftButtonDown == 0)
    {
    return;
    }

  // stop adjusting
  this->State = SVTK_Actor2DWidget::Outside;
  this->EventCallbackCommand->SetAbortFlag(1);
  this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_DEFAULT);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  this->LeftButtonDown = 0;
}

void SVTK_Actor2DWidget::OnRightButtonDown()
{

  // are we not over the scalar bar, ignore
  if (this->State == SVTK_Actor2DWidget::Outside)
    {
    return;
    }

  if (this->HasObserver(vtkCommand::RightButtonPressEvent) ) 
    {
    this->EventCallbackCommand->SetAbortFlag(1);
    this->InvokeEvent(vtkCommand::RightButtonPressEvent,NULL);
    }
  RightButtonDown = 1;
}

void SVTK_Actor2DWidget::OnRightButtonUp()
{
  if (RightButtonDown == 0) {
    return;
  }

  if (this->HasObserver(vtkCommand::RightButtonReleaseEvent)) 
    {
    this->EventCallbackCommand->SetAbortFlag(1);
    this->InvokeEvent(vtkCommand::RightButtonReleaseEvent,NULL);
    }
  this->RightButtonDown = 0;
}

void SVTK_Actor2DWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "Actor2D: " << this->Actor2D << "\n";
}
