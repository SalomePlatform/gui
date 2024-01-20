// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

// File   : LightApp_ModuleAction.h
// Author : Sergey TELKOV, Vadim SANDLER
//
#ifndef LIGHTAPP_MODULEACTION_H
#define LIGHTAPP_MODULEACTION_H

#include "LightApp.h"

#include <QtxAction.h>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class QtxComboBox;
class QtxResourceMgr;
class QSignalMapper;

class LIGHTAPP_EXPORT LightApp_ModuleAction : public QtxAction
{
  Q_OBJECT

private:
  class ActionSet;
  class ComboAction;
  class ActivateEvent;

public:
  typedef enum { Buttons   = 0x01,
                 List      = 0x02,
                 AddRemove = 0x04,
                 All       = Buttons | List | AddRemove
  } Mode;

public:
  LightApp_ModuleAction( QtxResourceMgr*, QObject* = 0 );
  virtual ~LightApp_ModuleAction();

  int              count() const;
  QStringList      modules() const;

  QIcon            moduleIcon( const QString& ) const;
  void             setModuleIcon( const QString&, const QIcon& );

  QAction*         moduleAction( const QString& ) const;

  void             insertModule( const QString&, const QIcon&, const int = -1 );
  void             insertModule( const QString&, const QIcon&, bool, const int = -1 );
  void             removeModule( const QString& );
  void             insertExtension(const QString&);
  void             removeExtension(const QString&);
  void             updateExtActions();

  QString          activeModule() const;

  void             setMode( const Mode& );
  void             setModeEnabled( const Mode&, bool );
  bool             isModeEnabled( const Mode& ) const;

public slots:
  void             setActiveModule( const QString& );


protected:
  virtual void     addedTo( QWidget* );
  virtual void     removedFrom( QWidget* );

  virtual bool     event( QEvent* );

signals:
  void             moduleActivated( const QString& );
  void             adding();
  void             removing( const QString& );
  void             showExtInfo();

private:
  void             update();
  void             update( QtxComboBox* );

  QAction*         active() const;
  void             activate( int, bool = true );

private slots:
  void             onChanged();
  void             onTriggered( int );
  void             onComboActivated( int );

private:
  ComboAction*     myCombo;
  QtxAction*       myAdd;
  QtxAction*       myRemove;
  QtxAction*       myInfo;
  ActionSet*       mySet;
  QAction*         mySeparator;
  int              myMode;
  QSignalMapper*   myMapper;
};

class LightApp_ModuleAction::ComboAction : public QtxAction
{
  Q_OBJECT

public:
  ComboAction( QObject* parent );

  QList<QtxComboBox*> widgets() const;

signals:
  void                activatedId( int );

protected:
  virtual QWidget*    createWidget( QWidget* );
};

#endif // LIGHTAPP_MODULEACTION_H
