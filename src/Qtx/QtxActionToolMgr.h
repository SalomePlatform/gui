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
// File:      QtxActionToolMgr.h
// Author:    Alexander SOLOVYOV, Sergey TELKOV

#ifndef QTXACTIONTOOLMGR_H
#define QTXACTIONTOOLMGR_H

#include "QtxActionMgr.h"

#include <QMap>
#include <QList>

class QToolBar;
class QMainWindow;
class QAction;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxActionToolMgr : public QtxActionMgr
{
  Q_OBJECT

  class ToolNode
  {
  public:
    ToolNode() : visible( true ) {};
    ToolNode( const QtxActionMgrId& _id ) : id( _id ), visible( true ) {};

    QtxActionMgrId id;          //!< tool node ID
    bool           visible;     //!< visibility status
  };

  typedef QList<ToolNode> NodeList;    //!< toolbar nodes list

protected:
  class ToolCreator;

public:
  QtxActionToolMgr( QMainWindow* );
  virtual ~QtxActionToolMgr();

  QMainWindow*    mainWindow() const;

  QtxActionMgrId  createToolBar( const QString&, const QtxActionMgrId& = QtxActionMgrId(), QMainWindow* = 0 );
  void            removeToolBar( const QString& );
  void            removeToolBar( const QtxActionMgrId& );

  QtxActionMgrId  insert( const QtxActionMgrId&, const QtxActionMgrId&, const int = -1 );
  QtxActionMgrId  insert( QAction*, const QtxActionMgrId&, const int = -1 );
  QtxActionMgrId  insert( const QtxActionMgrId&, const QString&, const int = -1 );
  QtxActionMgrId  insert( QAction*, const QString&, const int = -1 );

  virtual bool    isVisible( const QtxActionMgrId&, const QtxActionMgrId& ) const;
  virtual void    setVisible( const QtxActionMgrId&, const QtxActionMgrId&, const bool );

  bool            isShown( const QtxActionMgrId& ) const;
  void            setShown( const QtxActionMgrId&, const bool );

  void            remove( const QtxActionMgrId&, const QtxActionMgrId& );
  void            remove( const QtxActionMgrId&, const QString& );

  QToolBar*       toolBar( const QtxActionMgrId& ) const;
  QToolBar*       toolBar( const QString& ) const;
  
  bool            hasToolBar( const QtxActionMgrId& ) const;
  bool            hasToolBar( const QString& ) const;

  bool            containsAction( const QtxActionMgrId&, const QtxActionMgrId& = QtxActionMgrId() ) const;

  QList<QtxActionMgrId> toolBars() const;
  QList<QtxActionMgrId> toolBarActions( const QtxActionMgrId& ) const;

  //  virtual bool    load( const QString&, QtxActionMgr::Reader& );

  QtxActionMgrId  find( QToolBar* ) const;

protected slots:
  void            onToolBarDestroyed();

protected:
  QtxActionMgrId  find( const QString& ) const;
  QToolBar*       find( const QString&, QMainWindow* ) const;

  virtual void    internalUpdate();
  void            updateToolBar( const QtxActionMgrId& );

  virtual void    updateContent();

  virtual bool    retrieve( const QMap<QtxActionMgrId, QtxActionMgrIO::Node>& );
  virtual bool    store( QMap<QtxActionMgrId, QtxActionMgrIO::Node>& ) const;

private:
  void            simplifySeparators( QToolBar* );
  void            triggerUpdate( const QtxActionMgrId& );

private:
  typedef struct { NodeList nodes; QToolBar* toolBar; } ToolBarInfo;   //!< toolbar info
  typedef QMap<int, ToolBarInfo>                        ToolBarMap;    //!< toolbars map

private:
  ToolBarMap                 myToolBars;      //!< toobars map
  QMainWindow*               myMainWindow;    //!< parent main window
  QMap<QtxActionMgrId, int>  myUpdateIds;     //!< list of actions ID being updated
};

class QtxActionToolMgr::ToolCreator : public QtxActionMgr::Creator
{
public:
  ToolCreator( QtxActionMgr::Reader*, QtxActionToolMgr* );
  virtual ~ToolCreator();

  virtual int append( const QString&, const bool,
                      const ItemAttributes&, const int );

private:
  QtxActionToolMgr* myMgr;         //!< toolbar manager
};

#endif
