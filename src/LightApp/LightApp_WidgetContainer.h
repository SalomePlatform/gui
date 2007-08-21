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
// File   : LightApp_WidgetContainer.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
// 

#ifndef LIGHTAPP_WIDGETCONTAINER_H
#define LIGHTAPP_WIDGETCONTAINER_H

#include "LightApp.h"

#include <QDockWidget>
#include <QMap>

class QWidget;
class QStackedWidget;

class LIGHTAPP_EXPORT LightApp_WidgetContainer : public QDockWidget
{
    Q_OBJECT

public:
  LightApp_WidgetContainer( const int, QWidget* = 0 );
  virtual ~LightApp_WidgetContainer();

  int                 type() const;

  int                 insert( const int, QWidget* );
  void                remove( const int, const bool = true );
  void                remove( QWidget*, const bool = true );
  bool                contains( const int ) const;
  bool                contains( QWidget* ) const;
  bool                isEmpty() const;

  void                activate( const int );
  void                activate( QWidget* );

  QWidget*            widget( const int ) const;
  QWidget*            active() const;

public slots:
  virtual void        setVisible( bool );

signals:
  void                visibilityChanged( bool );

private slots:
  void                onDestroyed( QObject* );

private:
 void                 updateTitle();
 
private:
  int                 myType;
  QStackedWidget*     myStack;
  QMap<int, QWidget*> myMap;
};

#endif  // LIGHTAPP_WIDGETCONTAINER_H
