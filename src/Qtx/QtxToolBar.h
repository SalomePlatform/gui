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

// File:      QtxToolBar.h
// Author:    Sergey TELKOV
//
#ifndef QTXTOOLBAR_H
#define QTXTOOLBAR_H

#include "Qtx.h"

#include <QToolBar>

class QTX_EXPORT QtxToolBar : public QToolBar
{
  Q_OBJECT

  class Watcher;

public:
  QtxToolBar( const bool, const QString&, QWidget* );
  QtxToolBar( const QString&, QWidget* );
  QtxToolBar( const bool, QWidget* = 0 );
  QtxToolBar( QWidget* = 0 );
  virtual ~QtxToolBar();

  QMainWindow*  mainWindow() const;

public slots:
  virtual void  setVisible( bool );

protected:
  virtual bool  event( QEvent* );

private:
  Watcher*      myWatcher;   //!< watcher object
  bool          myStretch;   //!< stretching toolbar flag (not used)
};

#endif // QTXTOOLBAR_H
