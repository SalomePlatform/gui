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

#ifndef SVTK_SELECTIONCHANGED_H
#define SVTK_SELECTIONCHANGED_H

#include <qobject.h>

#include "SVTK.h"
#include "SUIT_ViewModel.h"


//! To define minimal interface for #SVTK_ViewWindow initialization
class SVTK_EXPORT SVTK_ViewModelBase: public SUIT_ViewModel
{
  Q_OBJECT

public slots:
  //! To invoke Qt #selectionChanged signal
  void onSelectionChanged()
  {
    emit selectionChanged();
  }

 signals:
  //! To declare signal on selection changed
  void selectionChanged();
};

#endif
