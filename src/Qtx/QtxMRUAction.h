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
// File:      QtxMRUAction.h
// Author:    Sergey TELKOV

#ifndef QTXMRUACTION_H
#define QTXMRUACTION_H

#include "QtxAction.h"

#include <QMap>
#include <QStringList>

class QtxResourceMgr;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxMRUAction : public QtxAction
{
  Q_OBJECT

  Q_PROPERTY( int visibleCount READ visibleCount WRITE setVisibleCount )

public:
  enum { MoveFirst, MoveLast, AddFirst, AddLast };

public:
  QtxMRUAction( QObject* = 0 );
  QtxMRUAction( const QString&, const QString&, QObject* = 0 );
  QtxMRUAction( const QString&, const QIcon&, const QString&, QObject* = 0 );
  virtual ~QtxMRUAction();

  int          insertMode() const;
  void         setInsertMode( const int );

  int          count() const;
  bool         isEmpty() const;

  int          visibleCount() const;
  void         setVisibleCount( const int );

  void         remove( const int );
  void         remove( const QString& );
  void         insert( const QString& );

  QString      item( const int ) const;
  int          find( const QString& ) const;
  bool         contains( const QString& ) const;

  virtual void loadLinks( QtxResourceMgr*, const QString&, const bool = true );
  virtual void saveLinks( QtxResourceMgr*, const QString&, const bool = true ) const;

Q_SIGNALS:
  void         activated( const QString& );

private slots:
  void         onActivated();
  void         onAboutToShow();

private:
  void         updateMenu();

private:
  QStringList  myLinks;
  int          myVisCount;
  int          myInsertMode;
};

#endif
