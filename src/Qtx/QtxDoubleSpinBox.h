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
// File:      QtxDoubleSpinBox.h
// Author:    Sergey TELKOV

#ifndef QTXDOUBLESPINBOX_H
#define QTXDOUBLESPINBOX_H

#include "Qtx.h"

#include <QDoubleSpinBox>

class QTX_EXPORT QtxDoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT

public:
  QtxDoubleSpinBox( QWidget* = 0 );
  QtxDoubleSpinBox( double, double, double = 1, QWidget* = 0 );
  virtual ~QtxDoubleSpinBox();

  bool            isCleared() const;
  virtual void    setCleared( const bool );

  virtual void    stepBy( int );

private slots:
  virtual void    onTextChanged( const QString& );

protected:
  virtual QString textFromValue( double ) const;
  
  QString         removeTrailingZeroes( const QString& ) const;

private:
  bool            myCleared;
};

#endif
