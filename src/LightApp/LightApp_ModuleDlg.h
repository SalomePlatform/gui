// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
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

// File   : LightApp_ModuleDlg.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef LIGHTAPP_MODULEDLG_H
#define LIGHTAPP_MODULEDLG_H

#include "LightApp.h"

#include <QDialog> 
#include <QPixmap>
#include <QMap>

class QPushButton;
class QHBoxLayout;

class LIGHTAPP_EXPORT LightApp_ModuleDlg : public QDialog
{
  Q_OBJECT

public:
  LightApp_ModuleDlg( QWidget*, const QString&, const QPixmap& = QPixmap() ) ;
  ~LightApp_ModuleDlg();

  int addButton( const QString&, const int = -1);

public slots:
  void accept();

private:
  QPushButton* findButton( const int ) const;

private:
  typedef QMap<QPushButton*,int> ButtonMap;

private:
  ButtonMap    myButtons;
  QHBoxLayout* myButtonLayout;
};

#endif // LIGHTAPP_MODULEDLG_H

