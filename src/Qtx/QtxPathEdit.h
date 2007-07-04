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
// File:      QtxPathEdit.h
// Author:    Sergey TELKOV

#ifndef QTXPATHEDIT_H
#define QTXPATHEDIT_H

#include "Qtx.h"

#include <QFrame>

class QLineEdit;

class QTX_EXPORT QtxPathEdit : public QFrame
{
  Q_OBJECT

public:
  typedef enum { OpenFile, SaveFile, Directory } Mode;

public:
  QtxPathEdit( const int, QWidget* = 0 );
  QtxPathEdit( QWidget* = 0 );
  virtual ~QtxPathEdit();

  int          mode() const;
  void         setMode( const int );

  QString      path() const;
  void         setPath( const QString& );

  QString      filter() const;
  void         setFilter( const QString& );

private slots:
  void         onBrowse( bool = false );

protected:
  QLineEdit*   lineEdit() const;

private:
  void         initialize();
  void         updateState();

private:
  int          myMode;
  QLineEdit*   myPath;
  QString      myFilter;
};

#endif
