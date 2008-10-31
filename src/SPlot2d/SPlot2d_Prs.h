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
//  File   : SPlot2d_Prs.h
//  Author : Sergey RUIN
//  Module : SUIT

#ifndef SPLOT2D_PRS_H
#define SPLOT2D_PRS_H

#include "SPlot2d.h"
#include "SALOME_Prs.h"
#include "Plot2d_Prs.h"

class Plot2d_Curve;

class SPLOT2D_EXPORT SPlot2d_Prs : public SALOME_Prs2d, public Plot2d_Prs
{
public:

  // Default constructor 
  SPlot2d_Prs();

  // Standard constructor    
  SPlot2d_Prs( const Plot2d_Curve* obj );

  SPlot2d_Prs( const Plot2d_Prs* prs );

  // Destructor
  ~SPlot2d_Prs();

  virtual bool IsNull() const { return Plot2d_Prs::IsNull(); }

  // Get curves list
  // Note: Depricated method, use method getCurves     
  curveList GetObjects() const { return getCurves(); }

};

#endif