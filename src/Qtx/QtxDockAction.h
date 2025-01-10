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

// File:      QtxDockAction.h
// Author:    Sergey TELKOV
//
#ifndef QTXDOCKACTION_H
#define QTXDOCKACTION_H

#include "QtxAction.h"

#include <QList>

class QIcon;
class QString;
class QToolBar;
class QDockWidget;
class QMainWindow;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxDockAction : public QtxAction
{
  Q_OBJECT

public:
  //! Dock windows type.
  enum { ToolBar,      //!< list toolbars only
         DockWidget,   //!< list dockable windows only
         Both          //!< list toolbars and dockable windows
  } DockType;

public:
  QtxDockAction( QMainWindow* );
  QtxDockAction( const QString&, const QString&, QMainWindow* );
  QtxDockAction( const QString&, const QIcon&, const QString&, QMainWindow* );
  virtual ~QtxDockAction();

  void         update();

  int          dockType() const;
  void         setDockType( const int );

  QMainWindow* mainWindow() const;

private slots:
  void         onAboutToShow();

protected:
  virtual void addedTo( QWidget* );
  virtual void removedFrom( QWidget* );

private:
  void         initialize();
  void         updateMenu();

  void         toolBars( QList<QToolBar*>& ) const;
  void         dockWidgets( QList<QDockWidget*>& ) const;

private:
  int          myType;   //!< dock action type
  QMainWindow* myMain;   //!< parent main window
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // QTXDOCKACTION_H
