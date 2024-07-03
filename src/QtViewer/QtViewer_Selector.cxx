// Copyright (C) 2013-2023  CEA, EDF, OPEN CASCADE
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

#include "QtViewer_Selector.h"
#include "QtViewer_Viewer.h"

//=======================================================================
// Name    : QtViewer_Selector
// Purpose : Constructor
//=======================================================================
QtViewer_Selector::QtViewer_Selector(QtViewer_Viewer* theViewer)
: QObject( nullptr ),
  myViewer(theViewer)
{   
}

//=======================================================================
// Name    : QtViewer_Selector
// Purpose : Destructor
//=======================================================================
QtViewer_Selector::~QtViewer_Selector()
{
}

//================================================================
// Function : select
// Purpose  : 
//================================================================
void QtViewer_Selector::select( const QRectF& selRect)
{
  emit selSelectionDone();
}