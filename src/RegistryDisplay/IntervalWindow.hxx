//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME RegistryDisplay : GUI for Registry server implementation
//  File   : IntervalWindow.hxx
//  Author : Oksana TCHEBANOVA
//  Module : SALOME
//
#ifndef IntervalWindow_HeaderFile
#define IntervalWindow_HeaderFile

# include <qwidget.h>
# include <qdialog.h>
# include <qpushbutton.h>
# include <qspinbox.h>

class IntervalWindow : public QDialog
{
  Q_OBJECT

public:
  IntervalWindow( QWidget* parent = 0 );
  ~IntervalWindow();
  
  QPushButton* Ok();
  QPushButton* Cancel();

  int getValue();
  void setValue( int );

private:
  QSpinBox* mySpinBox;
  QPushButton* myButtonOk;
  QPushButton* myButtonCancel;
};

#endif
