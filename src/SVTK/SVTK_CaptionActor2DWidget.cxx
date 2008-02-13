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

#include "SVTK_CaptionActor2DWidget.h"

#include <vtkObjectFactory.h>

#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkCoordinate.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro(SVTK_CaptionActor2DWidget);

SVTK_CaptionActor2DWidget::SVTK_CaptionActor2DWidget()
{
  this->CaptionActor2D = NULL;
}

SVTK_CaptionActor2DWidget::~SVTK_CaptionActor2DWidget()
{
  this->SetCaptionActor2D(0);
}

void 
SVTK_CaptionActor2DWidget
::SetCaptionActor2D(vtkCaptionActor2D *theCaptionActor2D)
{
  this->SetActor2D(theCaptionActor2D);
  CaptionActor2D = theCaptionActor2D;
}

//----------------------------------------------------------------------------
namespace SVTK
{
  SVTK_CaptionActor2DWidget*
  PublishCaptionActor2D(vtkRenderWindowInteractor* theInteractor)
  {
    vtkCaptionActor2D* aCaptionActor2D = vtkCaptionActor2D::New();
    aCaptionActor2D->SetPickable(true); 
    aCaptionActor2D->SetBorder(true);
    aCaptionActor2D->SetLeader(false);
    aCaptionActor2D->SetCaption("");

    vtkCoordinate* aCoordinate = aCaptionActor2D->GetPositionCoordinate();
    aCoordinate->SetCoordinateSystemToNormalizedViewport();
    aCoordinate->SetReferenceCoordinate(NULL);
    aCoordinate->SetCoordinateSystemToNormalizedViewport();
    aCoordinate->SetValue(0.05, 0.05);
    aCaptionActor2D->SetWidth(0.25);
    aCaptionActor2D->SetHeight(0.15);

    vtkTextProperty *aTextProperty = vtkTextProperty::New();
    aCaptionActor2D->SetCaptionTextProperty(aTextProperty);
    aTextProperty->SetJustificationToCentered();
    aTextProperty->SetVerticalJustificationToCentered();
    aTextProperty->SetFontSize(10);
    aTextProperty->Delete();

    SVTK_CaptionActor2DWidget* aWidget = SVTK_CaptionActor2DWidget::New();
    aWidget->SetCaptionActor2D(aCaptionActor2D);
    aWidget->SetInteractor(theInteractor);
    aWidget->SetEnabled(1);

    return aWidget;
  }
}
