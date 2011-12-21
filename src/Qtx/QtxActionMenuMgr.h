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
// File:      QtxActionMenuMgr.h
// Author:    Alexander SOLOVYOV, Sergey TELKOV

#ifndef QTXACTIONMENUMGR_H
#define QTXACTIONMENUMGR_H

#include "Qtx.h"
#include "QtxActionMgr.h"

#include <QList>

class QMenu;
class QMainWindow;
class QStringList;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxActionMenuMgr : public QtxActionMgr
{
  Q_OBJECT

  class MenuNode;

  typedef QList<MenuNode*> NodeList;  //!< menu nodes list

protected:
  class MenuCreator;

public:
  QtxActionMenuMgr( QMainWindow* );
  QtxActionMenuMgr( QWidget*, QObject* );
  virtual ~QtxActionMenuMgr();

  QWidget*     menuWidget() const;

  virtual bool isVisible( const QtxActionMgrId&, const QtxActionMgrId& ) const;
  virtual void setVisible( const QtxActionMgrId&, const QtxActionMgrId&, const bool );

  QtxActionMgrId           insert( const QtxActionMgrId&, const QString&, const int, const int = -1 );
  QtxActionMgrId           insert( QAction*, const QString&, const int, const int = -1 );

  QtxActionMgrId           insert( const QtxActionMgrId&, const QStringList&, const int, const int = -1 );
  QtxActionMgrId           insert( QAction*, const QStringList&, const int, const int = -1 );

  virtual QtxActionMgrId   insert( const QtxActionMgrId&, const QtxActionMgrId&, const int, const int = -1 );
  QtxActionMgrId           insert( QAction*, const QtxActionMgrId&, const int, const int = -1 );

  QtxActionMgrId           insert( const QString&, const QString&, const int, const QtxActionMgrId& = QtxActionMgrId(), const int = -1 );
  QtxActionMgrId           insert( const QString&, const QStringList&, const int, const QtxActionMgrId& = QtxActionMgrId(), const int = -1 );
  virtual QtxActionMgrId   insert( const QString&, const QtxActionMgrId&, const int, const QtxActionMgrId& = QtxActionMgrId(), const int = -1 );

  void         remove( const QtxActionMgrId& );
  void         remove( const QtxActionMgrId&, const QtxActionMgrId&, const int = -1 );

  bool         isShown( const QtxActionMgrId& ) const;
  void         setShown( const QtxActionMgrId&, const bool );

  virtual void change( const QtxActionMgrId&, const QString& );

  virtual bool load( const QString&, QtxActionMgr::Reader& );

  bool         containsMenu( const QString&, const QtxActionMgrId&, const bool = false ) const;
  bool         containsMenu( const QtxActionMgrId&, const QtxActionMgrId&, const bool = false ) const;

  QMenu*       findMenu( const QtxActionMgrId& ) const;
  QMenu*       findMenu( const QString&, const QtxActionMgrId&, const bool = false ) const;

  bool         isEmptyEnabled( const QtxActionMgrId& ) const;
  void         setEmptyEnabled( const QtxActionMgrId&, const bool );

  bool         menuCollapsible() const;
  void         setMenuCollapsible( bool );

  int          menuPriority( const QtxActionMgrId& ) const;
  void         setMenuPriority( const QtxActionMgrId&, const int );

  bool         isPermanentMenu( const QtxActionMgrId& ) const;
  void         setPermanentMenu( const QtxActionMgrId&, bool );

private slots:
  void         onAboutToShow();
  void         onAboutToHide();
  void         onDestroyed( QObject* );

signals:
  void         menuAboutToShow( QMenu* );
  void         menuAboutToHide( QMenu* );

protected:
  void         setMenuWidget( QWidget* );

  MenuNode*    find( const QtxActionMgrId&, const QtxActionMgrId&, const bool = true ) const;
  MenuNode*    find( const QtxActionMgrId&, MenuNode* = 0, const bool = true ) const;
  bool         find( const QtxActionMgrId&, NodeList&, MenuNode* = 0 ) const;
  MenuNode*    find( const QString&, const QtxActionMgrId&, const bool = true ) const;
  MenuNode*    find( const QString&, MenuNode* = 0, const bool = true ) const;
  bool         find( const QString&, NodeList&, MenuNode* = 0 ) const;
  QtxActionMgrId findId( const QtxActionMgrId&, const QtxActionMgrId& = QtxActionMgrId() ) const;

  void         removeMenu( const QtxActionMgrId&, MenuNode* );

  QAction*     itemAction( const QtxActionMgrId& ) const;
  QAction*     menuAction( const QtxActionMgrId& ) const;
  QtxActionMgrId menuActionId( QAction* ) const;

  void         updateMenu( MenuNode* = 0, const bool = true, const bool = true );
  virtual void internalUpdate();
  virtual void updateContent();

private:
  bool         ownAction( QAction*, MenuNode* ) const;
  bool         checkWidget( QWidget* ) const;
  QWidget*     menuWidget( MenuNode* ) const;
  void         simplifySeparators( QWidget* );
  QString      clearTitle( const QString& ) const;
  QtxActionMgrId createMenu( const QStringList&, const QtxActionMgrId& );

  void         triggerUpdate( const QtxActionMgrId&, const bool rec = true );

private:
  typedef QMap<int, QAction*> MenuMap;     //!< actions map

private:
  MenuNode*                   myRoot;        //!< root menu node
  QWidget*                    myMenu;        //!< menu widget
  MenuMap                     myMenus;       //!< actions map
  bool                        myCollapse;
  QMap<QtxActionMgrId, bool>  myUpdateIds;   //!< list of actions ID being updated
};

class QtxActionMenuMgr::MenuCreator : public QtxActionMgr::Creator
{
public:
  MenuCreator( QtxActionMgr::Reader*, QtxActionMenuMgr* );
  virtual ~MenuCreator();

  virtual int append( const QString&, const bool,
                      const ItemAttributes&, const int );

private:
  QtxActionMenuMgr* myMgr;       //!< menu manager
};


#endif
