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

#ifndef STD_SDIDESKTOP_H
#define STD_SDIDESKTOP_H

#include "STD.h"

#include <SUIT_Desktop.h>

class QFrame;

class STD_EXPORT STD_SDIDesktop: public SUIT_Desktop 
{
  Q_OBJECT

public:
  STD_SDIDesktop();
  virtual ~STD_SDIDesktop();

  virtual SUIT_ViewWindow* activeWindow() const;
  virtual QList<SUIT_ViewWindow*> windows() const;

protected:
  virtual void             addWindow( QWidget* );

private:
  QFrame*                  myMainWidget;
};

#endif
