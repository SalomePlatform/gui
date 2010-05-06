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
// File:      QtxDockWidget.h
// Author:    Sergey TELKOV

#ifndef QTXDOCKWIDGET_H
#define QTXDOCKWIDGET_H

#include "Qtx.h"

#include <QDockWidget>

class QTX_EXPORT QtxDockWidget : public QDockWidget
{
  Q_OBJECT

  class Watcher;

public:

  enum TitleWidgetRole { 
    TitleText,
    Undock, 
    Close,
    Last
  };

  QtxDockWidget( const QString&, QWidget* = 0, Qt::WindowFlags = 0 );
  QtxDockWidget( const bool, QWidget* = 0, Qt::WindowFlags = 0 );
  QtxDockWidget( QWidget*, Qt::WindowFlags = 0 );
  virtual ~QtxDockWidget();

  virtual QSize   sizeHint() const;
  virtual QSize   minimumSizeHint() const;

  Qt::Orientation orientation() const;

  void            enableCustomTitleBar( const bool );
  bool            customTitleBarEnabled() const;
  QWidget*        customTitleBarWidget( const int ) const;
  int             customTitleBarRole( QWidget* ) const;
  void            insertCustomTitleBarWidget( const int, QWidget*, const int = Last );
  void            removeCustomTitleBarWidget( const int );

  void            updateCustomTitleBar();

signals:
  void            orientationChanged( Qt::Orientation );

public slots:
  virtual void    setVisible( bool );
  void            toggleTopLevel();

protected:
  virtual void    resizeEvent( QResizeEvent* );

private:
  void            updateState();

private:
  Watcher*        myWatcher;       //!< watcher object
  Qt::Orientation myOrientation;   //!< dockable window orientation
};

#endif // QTXDOCKWIDGET_H
