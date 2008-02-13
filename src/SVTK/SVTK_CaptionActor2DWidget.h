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

#ifndef __SVTK_CAPTION_ACTOR2D_WIDGET__
#define __SVTK_CAPTION_ACTOR2D_WIDGET__

#include "SVTK.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SVTK_Actor2DWidget.h"

class vtkCaptionActor2D;


//----------------------------------------------------------------------------
class SVTK_EXPORT SVTK_CaptionActor2DWidget : public SVTK_Actor2DWidget
{
public:
  static SVTK_CaptionActor2DWidget *New();
  vtkTypeMacro(SVTK_CaptionActor2DWidget, SVTK_Actor2DWidget);

  void SetCaptionActor2D(vtkCaptionActor2D *theCaptionActor2D);
  vtkGetObjectMacro(CaptionActor2D, vtkCaptionActor2D);
  
protected:
  SVTK_CaptionActor2DWidget();
  ~SVTK_CaptionActor2DWidget();

  // the actor that is used
  vtkCaptionActor2D *CaptionActor2D;

private:
  SVTK_CaptionActor2DWidget(const SVTK_CaptionActor2DWidget&);  //Not implemented
  void operator=(const SVTK_CaptionActor2DWidget&);  //Not implemented
};


//----------------------------------------------------------------------------
namespace SVTK
{
  SVTK_CaptionActor2DWidget*
  PublishCaptionActor2D(vtkRenderWindowInteractor* theInteractor);
}


//----------------------------------------------------------------------------
#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // __SVTK_CAPTION_ACTOR2D_WIDGET__
