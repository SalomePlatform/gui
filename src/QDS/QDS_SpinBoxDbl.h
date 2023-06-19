// Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
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

#ifndef QDS_PINBOXDBL_H
#define QDS_PINBOXDBL_H

#include "QDS_Datum.h"

class QtxDoubleSpinBox;

class QDS_EXPORT QDS_SpinBoxDbl : public QDS_Datum
{
  Q_OBJECT

public:
  QDS_SpinBoxDbl( const QString&, QWidget* = 0, const int = All, const QString& = QString() );
  virtual ~QDS_SpinBoxDbl();

  double            step() const;
  void              setStep( const double );

  virtual QString   getString() const;

private slots:
  void              onValueChanged( double );

protected:
  QtxDoubleSpinBox* spinBox() const;
  virtual QWidget*  createControl( QWidget* );

  virtual void      setString( const QString& );

  virtual void      unitSystemChanged( const QString& );
};

#endif 
