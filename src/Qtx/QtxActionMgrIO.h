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
// File:      QtxActionMgrIO.h
// Author:    Sergey TELKOV

#ifndef QTXACTIONMGRIO_H
#define QTXACTIONMGRIO_H

#include "QtxActionMgrId.h"

#include <QtCore/QMap>
#include <QtCore/QVariant>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QtxResourceMgr;

class QTX_EXPORT QtxActionMgrIO
{
public:
  class Node
  {
  public:
    Node() {}
    Node( const QtxActionMgrId& id ) : myId( id ) {}
    Node( const Node& node ) : myId( node.myId ), myData( node.myData ), myChildren( node.myChildren ) {}
    ~Node() {}

    QtxActionMgrId          id() const { return myId; }
    QList<QtxActionMgrId>   children() const { return myChildren; }
    void                    setChildren( const QList<QtxActionMgrId>& lst ) { myChildren = lst; }

    QStringList             data() const { return myData.keys(); }
    QVariant                data( const QString& name ) const { return myData.contains( name ) ? myData[name] : QVariant(); }
    void                    setData( const QString& name, const QVariant& val ) { myData.insert( name, val ); }

  private:
    QtxActionMgrId          myId;
    QMap<QString, QVariant> myData;
    QList<QtxActionMgrId>   myChildren;
  };

public:
  QtxActionMgrIO();
  virtual ~QtxActionMgrIO();

  QStringList    options() const;
  QString        option( const QString&, const QString& = QString() ) const;
  void           setOption( const QString&, const QString& );

  virtual bool   load( const QString&, QMap<QtxActionMgrId, Node>& ) const = 0;
  virtual bool   save( const QString&, const QMap<QtxActionMgrId, Node>& ) const = 0;

private:
  QMap<QString, QString> myOptions;
};

class QTX_EXPORT QtxActionMgrResIO : public QtxActionMgrIO
{
public:
  QtxActionMgrResIO( QtxResourceMgr* );
  virtual ~QtxActionMgrResIO();

  QtxResourceMgr* resourceMgr() const;

  virtual bool    load( const QString&, QMap<QtxActionMgrId, Node>& ) const;
  virtual bool    save( const QString&, const QMap<QtxActionMgrId, Node>& ) const;

private:
  QtxActionMgrId  nodeId( const QString& ) const;
  QString         nodeId( const QtxActionMgrId& ) const;

private:
  QtxResourceMgr* myResMgr;
};

#endif
