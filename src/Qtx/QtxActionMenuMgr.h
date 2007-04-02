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
// Author:    Alexander SOLOVYEV, Sergey TELKOV

#ifndef QTXACTIONMENUMGR_H
#define QTXACTIONMENUMGR_H

#include "Qtx.h"
#include "QtxActionMgr.h"

#include <QtCore/qlist.h>
#include <QtCore/qstringlist.h>

class QPopupMenu;
class QMainWindow;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class QtxActionMenuMgr
  Allows to use set of action to automatically build main menu.
  With help of methods insert/append/remove it is possible to 
  describe whole structure of menu. Method hide allows
  to temporary remove some items from menu, method show allows to 
  recreate them.
  Actions can be grouped with help of group identifictor.
  Inside popup or menu bar items have order by increasing group id.
  This manager is able to attune menu: to remove excess separators,
  to remove empty popup menu etc.
*/
class QTX_EXPORT QtxActionMenuMgr : public QtxActionMgr
{
  Q_OBJECT

  class MenuNode;

  typedef QList<MenuNode*> NodeList;

  /*!
    \class MenuNode
    Represents a menu item inside main menu structure.
    For internal purposes only
  */
  class MenuNode
  {
  public:
    MenuNode() : parent( 0 ), visible( true ) {};
    MenuNode( MenuNode* p ) : parent( p ), visible( true ) {};
    ~MenuNode()
    {
      for ( NodeList::iterator it = children.begin(); it != children.end(); ++it )
        delete *it;
    }

    int       id;
    int       idx;
    int       group;
    MenuNode* parent;
    bool      visible;
    NodeList  children;
  };

  class MenuAction;

protected:
  class MenuCreator;

public:
  QtxActionMenuMgr( QMainWindow* );
  QtxActionMenuMgr( QWidget*, QObject* );
  virtual ~QtxActionMenuMgr();

  virtual bool isVisible( const int, const int ) const;
  virtual void setVisible( const int, const int, const bool );

  int          insert( const int, const QString&, const int, const int = -1 );
  int          insert( QAction*, const QString&, const int, const int = -1 );

  int          insert( const int, const QStringList&, const int, const int = -1 );
  int          insert( QAction*, const QStringList&, const int, const int = -1 );

  virtual int  insert( const int, const int, const int, const int = -1 );
  int          insert( QAction*, const int, const int, const int = -1 );

  int          insert( const QString&, const QString&, const int, const int = -1, const int = -1, const bool = false );
  int          insert( const QString&, const QStringList&, const int, const int = -1, const int = -1, const bool = false );
  virtual int  insert( const QString&, const int, const int, const int = -1, const int = -1, const bool = false );

  int          append( const int, const int, const int );
  int          append( QAction*, const int, const int );
  int          append( const QString&, const int, const int, const int = -1, const bool = false );

  int          prepend( const int, const int, const int );
  int          prepend( QAction*, const int, const int );
  int          prepend( const QString&, const int, const int, const int = -1, const bool = false );

  void         remove( const int );
  void         remove( const int, const int, const int = -1 );

  void         show( const int );
  void         hide( const int );

  bool         isShown( const int ) const;
  void         setShown( const int, const bool );

  virtual void change( const int, const QString& );

  virtual bool load( const QString&, QtxActionMgr::Reader& );

  bool         containsMenu( const QString&, const int ) const;
  bool         containsMenu( const int, const int ) const;

private Q_SLOTS:
  void         onAboutToShow();
  void         onAboutToHide();
  void         onHighlighted( int );
  void         onDestroyed( QObject* );

Q_SIGNALS:
  void         menuAboutToShow( QMenu* );
  void         menuAboutToHide( QMenu* );

  void         menuHighlighted( int, int );

protected:
  void         setWidget( QWidget* );
  MenuNode*    find( const int, const int, const bool = true ) const;
  MenuNode*    find( const int, MenuNode* = 0, const bool = true ) const;
  bool         find( const int, NodeList&, MenuNode* = 0 ) const;
  MenuNode*    find( const QString&, const int, const bool = true ) const;
  MenuNode*    find( const QString&, MenuNode* = 0, const bool = true ) const;
  bool         find( const QString&, NodeList&, MenuNode* = 0 ) const;
  int          findId( const int, const int = -1 ) const;

  void         removeMenu( const int, MenuNode* );

  QAction*     itemAction( const int ) const;
  QAction*     menuAction( const int ) const;

  void         updateMenu( MenuNode* = 0, const bool = true, const bool = true );
  virtual void internalUpdate();  
  virtual void updateContent();

private:
  bool         checkWidget( QWidget* ) const;
  QWidget*     menuWidget( MenuNode* ) const;
  void         simplifySeparators( QWidget* );
  QString      clearTitle( const QString& ) const;
  int          createMenu( const QStringList&, const int );

  void         triggerUpdate( const int, const bool rec = true );

private:
  typedef QMap<int, QAction*> MenuMap;

private:
  MenuNode     myRoot;
  QWidget*     myMenu;
  MenuMap      myMenus;
  QMap<int, bool> myUpdateIds;
};

/*!
  \class QtxActionMenuMgr::MenuCreator
  Allows to create automatically main menu by data read from file
*/
class QtxActionMenuMgr::MenuCreator : public QtxActionMgr::Creator
{
public:
  MenuCreator( QtxActionMgr::Reader*, QtxActionMenuMgr* );
  virtual ~MenuCreator();

  virtual int append( const QString&, const bool,
                      const ItemAttributes&, const int );

private:
  QtxActionMenuMgr* myMgr;
};


#endif
