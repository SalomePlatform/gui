// Copyright (C) 2007-2023  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef VTKVIEWER_CELLCENTERS_H
#define VTKVIEWER_CELLCENTERS_H

#include "VTKViewer.h"

#include <vtkCellCenters.h>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*!
 * Class       : VTKViewer_CellCenters
 * Description : Filter computing geometrical centers of given cells
 *               (differs from native vtk filter by small fix for VTK_CONVEX_POINT_SET cells)
 */
class VTKVIEWER_EXPORT VTKViewer_CellCenters : public vtkCellCenters
{
public:
  vtkTypeMacro(VTKViewer_CellCenters,vtkCellCenters)

  static VTKViewer_CellCenters *New();

protected:
  VTKViewer_CellCenters();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  VTKViewer_CellCenters(const VTKViewer_CellCenters&);  // Not implemented.
  void operator=(const VTKViewer_CellCenters&);  // Not implemented.
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
