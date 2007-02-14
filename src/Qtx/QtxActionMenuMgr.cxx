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
// File:      QtxActionMenuMgr.cxx
// Author:    Alexander SOLOVYEV, Sergey TELKOV

#include "QtxActionMenuMgr.h"

#include "QtxAction.h"

#include <QtCore/qlist.h>
#include <QtCore/qfile.h>

#include <QtGui/qmenu.h>
#include <QtGui/qwidget.h>
#include <QtGui/qmenubar.h>
#include <QtGui/qmainwindow.h>

#include <QtXml/qdom.h>

// VSR: Uncomment this #define in order to allow dynamic menus support
// (emit signals when popup menu is pre-activated)
// Currently this support is disabled.
//#define ENABLE_DYNAMIC_MENU
 
/*!
	Service functions
	Level: Internal
*/
namespace {
/*
  QList<int> prepareIds( const QWidget* w )
  {
    QList<int> l;
    const QMenuData* md = 0;
    if ( w->inherits( "QMenuBar" ) )
      md = ::qt_cast<QMenuBar*>( w );
    else if ( w->inherits( "QPopupMenu" ) )
      md = ::qt_cast<QPopupMenu*>( w );
    if ( md )
    {
      for ( uint i = 0; i < md->count(); i++ )
	      l.append( md->idAt( i ) );
    }
    return l;
  }

  int getNewId( const QWidget* w, const QValueList<int>& l, bool retId = true )
  {
    const QMenuData* md = 0;
    if ( w->inherits( "QMenuBar" ) )
      md = ::qt_cast<QMenuBar*>( w );
    else if ( w->inherits( "QPopupMenu" ) )
      md = ::qt_cast<QPopupMenu*>( w );
    if ( md )
    {
      for ( uint i = 0, j = 0; i < md->count() && j < l.count(); i++, j++ )
	      if ( md->idAt( i ) != l[ j ] )
          return retId ? md->idAt( i ) : i;
      if ( md->count() > l.count() )
        return retId ? md->idAt( md->count()-1 ) : md->count() - 1;
    }
    return -1;
  }

  void dumpMenu( QWidget* w, bool before )
  {
    if ( !w )
      return;

    QMenuData* md = 0;
    if ( w->inherits( "QMenuBar" ) )
      md = ::qt_cast<QMenuBar*>( w );
    else if ( w->inherits( "QPopupMenu" ) )
      md = ::qt_cast<QPopupMenu*>( w );

    if ( !md )
      return;

    printf( ">>> start dump menu (%s) >>>\n", before ? "before" : "after" );
    for ( uint i = 0; i < md->count(); i++ )
      printf( "%d: %d: %s\n", i, md->idAt( i ), md->text( md->idAt( i ) ).latin1() );
    printf( "<<< end dump menu (%s) <<<\n", before ? "before" : "after" );
  }
*/
};

/*!
	Class: QtxActionMenuMgr::MenuAction
	Level: Internal
*/

class QtxActionMenuMgr::MenuAction : public QtxAction
{
public:
  MenuAction( const QString&, const QString&, QObject*, const int = -1, const bool = false );
  virtual ~MenuAction();

private:
  int          myId;
  bool         myEmptyEnabled;
};

/*!
  Constructor for menu action
  \param text - description text
  \param menutext - menu text
  \param parent - parent object
  \param id - integer identificator of action
  \param allowEmpty - if it is true, it makes possible to add this action with empty popup to menu
*/
/*
QtxActionMenuMgr::MenuAction::MenuAction( const QString& text, const QString& menuText,
					                                QObject* parent, const int id, const bool allowEmpty )
: QtxAction( text, menuText, 0, parent ),
myId( id ),
myPopup( 0 ),
myEmptyEnabled( allowEmpty )
{
  myPopup = new QMenu();
}
*/
/*!
  Destructor: deletes internal popup
*/
/*
QtxActionMenuMgr::MenuAction::~MenuAction()
{
  delete myPopup;
}
*/
/*!
  Adds action to widget, for example, to popup menu or menu bar
*/
/*
bool QtxActionMenuMgr::MenuAction::addTo( QWidget* w )
{
  if ( !w ) 
    return false;  // bad widget

  if ( !w->inherits( "QPopupMenu" ) && !w->inherits( "QMenuBar" ) )
    return false;  // not allowed widget type

  if ( myIds.find( w ) != myIds.end() )
    return false;  // already added

  if ( !myPopup )
    return false;  // bad own popup menu

  if ( !myEmptyEnabled && !myPopup->actions().count() )
    return false;  // not allowed empty menu

  if ( w->inherits( "QPopupMenu" )  )
  {
    QValueList<int> l = prepareIds( w );
    int idx;
    if ( QtxAction::addTo( w ) && ( idx = getNewId( w, l, false ) ) != -1 )
    {
      QPopupMenu* pm = (QPopupMenu*)w;
      myIds[ w ] = pm->idAt( idx );
      if ( myId != -1 ) 
	      pm->setId( idx, myId );
      setPopup( pm, myId != -1 ? myId : myIds[ w ], myPopup );
    }
  }
  else if ( w->inherits( "QMenuBar" ) )
  {
    QValueList<int> l = prepareIds( w );
    int idx;
    if ( QtxAction::addTo( w ) && ( idx = getNewId( w, l, false ) ) != -1 )
    {
      QMenuBar* mb = (QMenuBar*)w;
      myIds[ w ] = mb->idAt( idx );
      if ( myId != -1 ) 
    	  mb->setId( idx, myId );
      setPopup( mb, myId != -1 ? myId : myIds[ w ], myPopup );
    }

  }
  else
    return false;

  return true;
}
*/

/*!
  Removes action from widget, for example, from popup menu or menu bar
*/
/*
bool QtxActionMenuMgr::MenuAction::removeFrom( QWidget* w )
{
  if ( !w ) 
    return false;  // bad widget

  if ( !w->inherits( "QPopupMenu" ) && !w->inherits( "QMenuBar" ) )
    return false;  // not allowed widget type

  if ( myIds.find( w ) == myIds.end() )
    return false;  // not yet added

  if ( w->inherits( "QPopupMenu" ) )
  {
    if ( myId != -1 )
    {
      QPopupMenu* pm = (QPopupMenu*)w;
      int idx = pm->indexOf( myId );
      if ( idx != -1 )
        pm->setId( idx, myIds[ w ] );
    }
    myIds.remove( w );
    return QtxAction::removeFrom( w );;
  }
  else if ( w->inherits( "QMenuBar" ) )
  {
    if ( myId != -1 )
    {
      QMenuBar* mb = (QMenuBar*)w;
      int idx = mb->indexOf( myId );
      if ( idx != -1 )
        mb->setId( idx, myIds[ w ] );
    }
    myIds.remove( w );
    return QtxAction::removeFrom( w );
  }
  return false;
}
*/

/*!
  \return internal popup of action
*/
/*
QMenu* QtxActionMenuMgr::MenuAction::popup() const
{
  return myPopup;
}
*/

/*!
	Class: QtxActionMenuMgr
	Level: Public
*/
QtxActionMenuMgr::QtxActionMenuMgr( QMainWindow* p )
: QtxActionMgr( p ),
myMenu( p ? p->menuBar() : 0 )
{
  myRoot.id = -1;
  myRoot.group = -1;

  if ( myMenu ) {
    connect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
#ifdef ENABLE_DYNAMIC_MENU
    if ( myMenu->inherits( "QMenuBar" ) )
      connect( myMenu, SIGNAL( highlighted( int ) ), this, SLOT( onHighlighted( int ) ) );
#endif
  }
}

/*!
  Constructor
*/
QtxActionMenuMgr::QtxActionMenuMgr( QWidget* mw, QObject* p )
: QtxActionMgr( p ),
myMenu( mw )
{
  myRoot.id = -1;
  myRoot.group = -1;

  if ( myMenu )
    connect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
}

/*!
  Destructor
*/
QtxActionMenuMgr::~QtxActionMenuMgr()
{
  for ( NodeList::iterator it = myRoot.children.begin(); it != myRoot.children.end() && myMenu; ++it )
  {
    QAction* a = itemAction( (*it)->id );
    if ( !a )
      a = menuAction( (*it)->id );

//    if ( a )
//      a->removeFrom( myMenu );
  }

  for ( MenuMap::Iterator itr = myMenus.begin(); itr != myMenus.end(); ++itr )
    delete itr.value();
}

/*!
  \return whether menu item corresponding to action is visible
  \param actId - identificator of action
  \param place - identificator of some parent action
*/
bool QtxActionMenuMgr::isVisible( const int actId, const int place ) const
{
  MenuNode* node = find( actId, place );
  return node && node->visible;
}

/*!
  Sets visible state of action
  \param actId - identificator of action
  \param place - identificator of some parent action
  \param v - visibility state
*/
void QtxActionMenuMgr::setVisible( const int actId, const int place, const bool v )
{
  MenuNode* node = find( actId, place );
  if ( node )
    node->visible = v;
}

/*!
  Insert action as children menu item 
  \param id - identificator of action
  \param menus - few names of parent menu items, separated by '|'. It means sequence of menu items,
  for example "File|Edit" means File->Edit submenu. If submenu doesn't exist, it will be created.
  \param group - group identificator
  \param idx - index inside Qt menu
*/
int QtxActionMenuMgr::insert( const int id, const QString& menus, const int group, const int idx )
{
  return insert( id, menus.split( "|", QString::SkipEmptyParts ), group, idx );
}

/*!
  Insert action as children menu item 
  \param a - action
  \param menus - few names of parent menu items, separated by '|'. It means sequence of menu items,
  for example "File|Edit" means File->Edit submenu. If submenu doesn't exist, it will be created.
  \param group - group identificator
  \param idx - index inside Qt menu
*/
int QtxActionMenuMgr::insert( QAction* a, const QString& menus, const int group, const int idx )
{
  return insert( a, menus.split( "|", QString::SkipEmptyParts ), group, idx );
}

/*!
  Insert action as children menu item 
  \param id - identificator of action
  \param menus - list of names of parent menu items, separated by |. It means sequence of menu items,
  for example "File|Edit" means File->Edit submenu. If submenu doesn't exist, it will be created.
  \param group - group identificator
  \param idx - index inside Qt menu
*/
int QtxActionMenuMgr::insert( const int id, const QStringList& menus, const int group, const int idx )
{
  int pId = createMenu( menus, -1 );
  if ( pId == -1 )
    return -1;

  return insert( id, pId, group, idx );
}

/*!
  Insert action as children menu item 
  \param a - action
  \param menus - list of names of parent menu items. It means sequence of menu items,
  for example "File|Edit" means File->Edit submenu. If submenu doesn't exist, it will be created.
  \param group - group identificator
  \param idx - index inside Qt menu
*/
int QtxActionMenuMgr::insert( QAction* a, const QStringList& menus, const int group, const int idx )
{
  int pId = createMenu( menus, -1 );
  if ( pId == -1 )
    return -1;

  return insert( a, pId, group, idx );
}

/*!
  Insert action as children menu item 
  \param id - identificator of action
  \param pId - identificator of action corresponding to parent menu item
  \param group - group identificator
  \param idx - index inside Qt menu
*/
int QtxActionMenuMgr::insert( const int id, const int pId, const int group, const int idx )
{
  if ( id == -1 )
    return -1;

  MenuNode* pNode = pId == -1 ? &myRoot : find( pId );
  if ( !pNode )
    return -1;

  MenuNode* node = new MenuNode( pNode );
  node->id = id;
  node->idx = idx;
  node->group = group;

  pNode->children.append( node );

  triggerUpdate( pNode->id, false );


  return node->id;
}

/*!
  Insert action as children menu item 
  \param a - action
  \param pId - identificator of action corresponding to parent menu item
  \param group - group identificator
  \param idx - index inside Qt menu
*/
int QtxActionMenuMgr::insert( QAction* a, const int pId, const int group, const int idx )
{
  return insert( registerAction( a ), pId, group, idx );
}

/*!
  Create and insert action as children menu item 
  \return identificator of inserted action
  \param title - menu text of action
  \param pId - identificator of action corresponding to parent menu item
  \param group - group identificator
  \param id - identificator of new action
  \param idx - index inside Qt menu
  \param allowEmpty - indicates, that it is possible to add this action with empty popup menu to other menu
*/
int QtxActionMenuMgr::insert( const QString& title, const int pId, const int group, const int id, const int idx, const bool allowEmpty )
{
  MenuNode* pNode = pId == -1 ? &myRoot : find( pId );
  if ( !pNode )
    return -1;

  MenuNode* eNode = id == -1 ? 0 : find( id );

  int fid = -1;
  for ( NodeList::iterator it = pNode->children.begin(); it != pNode->children.end() && fid == -1; ++it )
  {
    if ( myMenus.contains( (*it)->id ) &&
         clearTitle( myMenus[(*it)->id]->text() ) == clearTitle( title ) )
      fid = (*it)->id;
  }

  if ( fid != -1 )
    return fid;

  int gid = (id == -1 || eNode ) ? generateId() : id;

  QAction* ma = new QAction( title, this );
  ma->setMenu( new QMenu( myMenu ) );
#ifdef ENABLE_DYNAMIC_MENU
  connect( ma->menu(), SIGNAL( highlighted( int ) ), this, SLOT( onHighlighted( int ) ) );
#endif

  MenuNode* node = new MenuNode( pNode );
  node->group = group;
  node->idx = idx;
  node->id = myMenus.insert( gid, ma ).key();

  pNode->children.append( node );

  triggerUpdate( pNode->id, false );

  return node->id;
}

/*!
  Create and insert action as children menu item 
  \return identificator of inserted action
  \param title - menu text of action
  \param menus - string list of parents' menu texts, separated by |
  \param group - group identificator
  \param id - identificator of new action
  \param idx - index inside Qt menu
  \param allowEmpty - indicates, that it is possible to add this action with empty popup menu to other menu
*/
int QtxActionMenuMgr::insert( const QString& title, const QString& menus, const int group, const int id, const int idx, const bool allowEmpty )
{
  return insert( title, menus.split( "|", QString::SkipEmptyParts ), group, id, idx, allowEmpty );
}

/*!
  Create and insert action as children menu item 
  \return identificator of inserted action
  \param title - menu text of action
  \param menus - list of parents menu items
  \param group - group identificator
  \param id - identificator of new action
  \param idx - index inside Qt menu
  \param allowEmpty - indicates, that it is possible to add this action with empty popup menu to other menu
*/
int QtxActionMenuMgr::insert( const QString& title, const QStringList& menus, const int group, const int id, const int idx, const bool allowEmpty )
{
  int pId = createMenu( menus, -1 );
  return insert( title, pId, group, id, idx, allowEmpty );
}

/*!
  Create and append action as last children
  \return identificator of inserted action
  \param title - menu text of action
  \param pId - id of action corresponding to parent menu item
  \param group - group identificator
  \param id - identificator of new action
  \param allowEmpty - indicates, that it is possible to add this action with empty popup menu to other menu
*/
int QtxActionMenuMgr::append( const QString& title, const int pId, const int group, const int id, const bool allowEmpty )
{
  return insert( title, pId, group, id, allowEmpty );
}

/*!
  Create and append action as last children
  \return identificator of inserted action
  \param id - identificator of existing action
  \param pId - id of action corresponding to parent menu item
  \param group - group identificator
*/
int QtxActionMenuMgr::append( const int id, const int pId, const int group )
{
  return insert( id, pId, group );
}

/*!
  Create and append action as last children
  \return identificator of inserted action
  \param a - action
  \param pId - id of action corresponding to parent menu item
  \param group - group identificator
*/
int QtxActionMenuMgr::append( QAction* a, const int pId, const int group )
{
  return insert( a, pId, group );
}

/*!
  Create and insert action as first children
  \return identificator of inserted action
  \param title - menu text of action
  \param pId - id of action corresponding to parent menu item
  \param group - group identificator
  \param id - identificator of new action
  \param allowEmpty - indicates, that it is possible to add this action with empty popup menu to other menu
*/
int QtxActionMenuMgr::prepend( const QString& title, const int pId, const int group, const int id, const bool allowEmpty )
{
  return insert( title, pId, group, id, 0, allowEmpty );
}

/*!
  Create and insert action as last children
  \return identificator of inserted action
  \param id - identificator of existing action
  \param pId - id of action corresponding to parent menu item
  \param group - group identificator
*/
int QtxActionMenuMgr::prepend( const int id, const int pId, const int group )
{
  return insert( id, pId, group, 0 );
}

/*!
  Create and insert action as last children
  \return identificator of inserted action
  \param a - action
  \param pId - id of action corresponding to parent menu item
  \param group - group identificator
*/
int QtxActionMenuMgr::prepend( QAction* a, const int pId, const int group )
{
  return insert( a, pId, group, 0 );
}

/*!
  Removes menu item corresponding to action
  \param id - identificator of action
*/
void QtxActionMenuMgr::remove( const int id )
{
  removeMenu( id, 0 );
  update();
}

/*!
  Removes menu item
  \param id - identificator of action
  \param pId - identificator of action corresponding to parent menu item
  \param group - group identificator
*/
void QtxActionMenuMgr::remove( const int id, const int pId, const int group )
{
  MenuNode* pNode = pId == -1 ? &myRoot : find( pId );
  if ( !pNode )
    return;

  NodeList delNodes;
  for ( NodeList::iterator it = pNode->children.begin(); it != pNode->children.end(); ++it )
  {
    if ( (*it)->id == id && ( (*it)->group == group || group == -1 ) )
      delNodes.append( *it );
  }

  for ( NodeList::iterator itr = delNodes.begin(); itr != delNodes.end(); ++itr )
    pNode->children.removeAll( *itr );

  triggerUpdate( pNode->id, false );
}

/*!
  Shows menu item corresponding to action
  \param id - identificator of action
*/
void QtxActionMenuMgr::show( const int id )
{
  setShown( id, true );
}

/*!
  Hides menu item corresponding to action
  \param id - identificator of action
*/
void QtxActionMenuMgr::hide( const int id )
{
  setShown( id, false );
}

/*!
  \return shown status of menu item corresponding to action
  \param id - identificator of action
*/
bool QtxActionMenuMgr::isShown( const int id ) const
{
  bool res = false;
  MenuNode* node = find( id );
  if ( node )
    res = node->visible;
  return res;
}

/*!
  Sets shown status of menu item corresponding to action
  \param id - identificator of action
  \param on - new shown status
*/
void QtxActionMenuMgr::setShown( const int id, const bool on )
{
  NodeList aNodes;
  find( id, aNodes );

  for ( NodeList::iterator it = aNodes.begin(); it != aNodes.end(); ++it )
  {
    if ( (*it)->visible != on )
    {
      (*it)->visible = on;
      triggerUpdate( (*it)->parent ? (*it)->parent->id : myRoot.id, false );
    }
  }
}

/*!
  Changes the specified menu title
*/
void QtxActionMenuMgr::change( const int id, const QString& title )
{
  QAction* a = menuAction( id );
  if ( a )
    a->setText( title );
}

/*!
  SLOT: called when corresponding menu is destroyed, clears internal pointer to menu
*/
void QtxActionMenuMgr::onDestroyed( QObject* obj )
{
  if ( myMenu == obj )
    myMenu = 0;
}

/*!
  SLOT: called when menu item is highlighted
*/
void QtxActionMenuMgr::onHighlighted( int id )
{
  const QObject* snd = sender();
  int pid = 0, realId;
  if ( myMenu && snd == myMenu )
    pid = -1;
  else
  {
    for ( MenuMap::Iterator itr = myMenus.begin(); itr != myMenus.end(); ++itr )
    {
      if ( itr.value()->menu() && itr.value()->menu() == snd )
	      pid = itr.key();
    }
  }
  if ( pid )
  {
    realId = findId( id, pid );
    if ( realId != -1 )
    {
      emit menuHighlighted( pid, realId );
      triggerUpdate( realId );
    }
  }
}

/*!
  Assignes new menu with manager
  \param mw - new menu
*/
void QtxActionMenuMgr::setWidget( QWidget* mw )
{
  if ( myMenu == mw )
    return;

  if ( myMenu )
    disconnect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  myMenu = mw;

  if ( myMenu )
    connect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
}

/*!
  \return menu node by it's place description
  \param actId - identificator of action
  \param pId - identificator of action corresponding to start menu item
  \param rec - recursive search
*/
QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const int actId, const int pId, const bool rec ) const
{
  return find( actId, find( pId ), rec );
}

/*!
  \return menu node by it's place description
  \param actId - identificator of action
  \param startNode - start menu item
  \param rec - recursive search
*/
QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const int id, MenuNode* startNode, const bool rec ) const
{
  MenuNode* node = 0;
  MenuNode* start = startNode ? startNode : (MenuNode*)&myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end() && !node; ++it )
  {
    if ( (*it)->id == id )
      node = *it;
    else if ( rec )
      node = find( id, *it, rec );
  }
  return node;
}

/*!
  Finds menu node
  \return true if at least one node is found
  \param id - identificator of action
  \param lst - list to be filled with found nodes
  \param startNode - start menu item
*/
bool QtxActionMenuMgr::find( const int id, NodeList& lst, MenuNode* startNode ) const
{
  MenuNode* start = startNode ? startNode : (MenuNode*)&myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end(); ++it )
  {
    MenuNode* node = *it;
    if ( node->id == id )
      lst.prepend( node );

    find( id, lst, node );
  }
  return !lst.isEmpty();
}

/*!
  Finds menu node
  \return menu node
  \param title - menu text of searched node
  \param pId - id of action corresponding to start menu item
  \param rec - recursive searching
*/
QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const QString& title, const int pId, const bool rec ) const
{
  return find( title, find( pId ), rec );
}

/*!
  Finds menu node
  \return true if at least one node is found
  \param title - menu text of node
  \param lst - list to be filled with found nodes
  \param startNode - start menu item
*/
bool QtxActionMenuMgr::find( const QString& title, NodeList& lst, MenuNode* startNode ) const
{
  MenuNode* start = startNode ? startNode : (MenuNode*)&myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end(); ++it )
  {
    QAction* a = itemAction( (*it)->id );
    if ( !a )
      a = menuAction( (*it)->id );
    if ( a && clearTitle( a->text() ) == clearTitle( title ) )
      lst.prepend( *it );

    find( title, lst, *it );
  }
  return !lst.isEmpty();
}

/*!
  Finds menu node
  \return menu node
  \param title - menu text of searched node
  \param startNode - start menu item
  \param rec - recursive searching
*/
QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const QString& title, MenuNode* startNode, const bool rec ) const
{
  MenuNode* node = 0;
  MenuNode* start = startNode ? startNode : (MenuNode*)&myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end() && !node; ++it )
  {
    QAction* a = itemAction( (*it)->id );
    if ( !a )
      a = menuAction( (*it)->id );
    if ( a && clearTitle( a->text() ) == clearTitle( title ) )
      node = *it;
    if ( !node && rec )
      node = find( title, *it, rec );
  }
  return node;
}

/*!
  Find id among children
  \return id (>0) if on success or -1 on fail
  \param id - id to be searched
  \param pid - id of parent, among children of that 'id' must be searched
*/
int QtxActionMenuMgr::findId( const int id, const int pid ) const
{
  MenuNode* start = pid != -1 ? find( pid ) : (MenuNode*)&myRoot;
  if ( start )
  {
    for ( NodeList::iterator it = start->children.begin(); it != start->children.end(); ++it )
    {
      if ( (*it)->id == id )
        return id;
    }
  }
  return -1;
}

/*!
  Removes child
  \param id - id of child to be removed
  \param startNode - parent menu item
*/
void QtxActionMenuMgr::removeMenu( const int id, MenuNode* startNode )
{
  MenuNode* start = startNode ? startNode : &myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end(); ++it )
  {
    if ( (*it)->id == id )
      start->children.removeAll( *it );
    else
      removeMenu( id, *it );
  }
}

/*!
  \return menu item action by id
  \param id - id of action
*/
QAction* QtxActionMenuMgr::itemAction( const int id ) const
{
  return action( id );
}

/*!
  \return menu action by id
  \param id - id of action
*/
QAction* QtxActionMenuMgr::menuAction( const int id ) const
{
  QAction* a = 0;

  if ( myMenus.contains( id ) )
    a = myMenus[id];

  return a;
}

/*!
  Updates menu ( isUpdatesEnabled() must return true )
  \param startNode - first menu item to be updated
  \param rec - recursive update
  \param updParent - update also parent item (without recursion)
  \sa isUpdatesEnabled()
*/
void QtxActionMenuMgr::updateMenu( MenuNode* startNode, const bool rec, const bool updParent )
{
  if ( !isUpdatesEnabled() )
    return;

  MenuNode* node = startNode ? startNode : &myRoot;

  QWidget* mw = menuWidget( node );
  if ( !mw )
    return;

  bool filled = checkWidget( mw );

  for ( NodeList::iterator it1 = node->children.begin(); it1 != node->children.end(); ++it1 )
  {
    QAction* a = itemAction( (*it1)->id );
    if ( !a )
      a = menuAction( (*it1)->id );

    mw->removeAction( a );
//    if ( a )
//      a->removeFrom( mw );
  }

  QMap<int, NodeList> idMap;
  for ( NodeList::iterator it2 = node->children.begin(); it2 != node->children.end(); ++it2 )
  {
    NodeList& lst = idMap[(*it2)->group];
    int idx = (*it2)->idx;
    if ( idx < 0 || idx >= (int)lst.count() )
      lst.append( *it2 );
    else
      lst.insert( idx, *it2 );
  }

  QIntList groups = idMap.keys();
  qSort( groups );

  groups.removeAll( -1 );
  groups.append( -1 );

  for ( QIntList::const_iterator gIt = groups.begin(); gIt != groups.end(); ++gIt )
  {
    if ( !idMap.contains( *gIt ) )
      continue;

    const NodeList& lst = idMap[*gIt];
    for ( NodeList::const_iterator iter = lst.begin(); iter != lst.end(); ++iter )
    {
      MenuNode* node = *iter;

      if ( rec )
        updateMenu( node, rec, false );

      MenuNode* par = node->parent;
      if ( !isVisible( node->id, par ? par->id : -1 ) )
        continue;

      QAction* a = itemAction( node->id );
      if ( !a )
        a = menuAction( node->id );

      if ( !a->menu() || !a->menu()->isEmpty() )
        mw->addAction( a );
    }
  }

  simplifySeparators( mw );

  if ( updParent && node->parent && filled != checkWidget( mw ) )
    updateMenu( node->parent, false );
}

/*!
  Updates menu (virtual variant). To be redefined for custom activity on menu updating
*/
void QtxActionMenuMgr::internalUpdate()
{
  if ( !isUpdatesEnabled() )
    return;

  updateMenu();
  myUpdateIds.clear();
}

/*!
  \return true if widget is non-empty menu
  \param wid - widget to be checked
*/
bool QtxActionMenuMgr::checkWidget( QWidget* wid ) const
{
  if ( !wid )
    return false;

  return !wid->actions().isEmpty();
}

/*!
  \return popup of menu item
  \param node - menu item
*/
QWidget* QtxActionMenuMgr::menuWidget( MenuNode* node) const
{
  if ( !node || node == &myRoot )
     return myMenu;

  if ( !myMenus.contains( node->id ) || !myMenus[node->id] )
    return 0;

  return myMenus[node->id]->menu();
}

/*!
  Removes excess separators of menu 
  \param wid - menu to be processed
*/
void QtxActionMenuMgr::simplifySeparators( QWidget* wid )
{
  Qtx::simplifySeparators( wid, false );
}

/*!
  Removes special symbols (&) from string
  \param txt - string to be processed
  \return clear variant of string
*/
QString QtxActionMenuMgr::clearTitle( const QString& txt ) const
{
  QString res = txt;

  for ( int i = 0; i < (int)res.length(); i++ )
  {
    if ( res.at( i ) == '&' )
      res.remove( i--, 1 );
  }

  return res;
}

/*!
  Creates and inserts many menu items 
  \param lst - list of menu texts
  \param pId - id of action corresponding to parent menu item
*/
int QtxActionMenuMgr::createMenu( const QStringList& lst, const int pId )
{
  if ( lst.isEmpty() )
    return -1;

  QStringList sl( lst );

  QString title = sl.last().trimmed();
  sl.removeLast();

  int parentId = sl.isEmpty() ? pId : createMenu( sl, pId );

  return insert( title, parentId, -1 );
}

/*!
  Loads actions description from file
  \param fname - name of file
  \param r - reader of file
  \return true on success
*/
bool QtxActionMenuMgr::load( const QString& fname, QtxActionMgr::Reader& r )
{
  MenuCreator cr( &r, this );
  return r.read( fname, cr );
}

/*!
  \return true if item has such child
  \param title - menu text of child
  \param pid - id of action corresponding to item
*/
bool QtxActionMenuMgr::containsMenu( const QString& title, const int pid ) const
{
  return (bool)find( title, pid, false );
}

/*!
  \return true if item has such child
  \param id - id of action corresponding to child
  \param pid - id of action corresponding to item
*/
bool QtxActionMenuMgr::containsMenu( const int id, const int pid ) const
{
  return (bool)find( id, pid, false );
}

/*!
  \Sets trigger for delayed update
*/
void QtxActionMenuMgr::triggerUpdate( const int id, const bool rec )
{
  bool isRec = rec;
  if ( myUpdateIds.contains( id ) )
    isRec = isRec || myUpdateIds[ id ];
  myUpdateIds.insert( id, isRec );

  QtxActionMgr::triggerUpdate();
}

/*!
  \Sets trigger for delayed update
*/
void QtxActionMenuMgr::updateContent()
{
  // Warning: For correct updating it is necessary to update the most enclosed submenu firstly
  //          because not updated empty submenu will be skipped. Now the submenu are iterated in
  //          ascending order their identifiers. For a submenu with automatically generated identifiers
  //          will work correctly as the uppermost submenu have the biggest number (identifiers generated
  //          reduction from -1). Generally when any submenu will have positive identifiers are obviously
  //          appropriated can to work not truly. In this case it is required to improve the given method
  //          and to add preliminary sorting a submenu on depth of an enclosure.
  for ( QMap<int, bool>::const_iterator it = myUpdateIds.constBegin(); it != myUpdateIds.constEnd(); ++it )
  {
    MenuNode* node = it.key() == -1 ? &myRoot : find( it.key() );
    if ( node )
      updateMenu( node, it.value() );
  }
  myUpdateIds.clear();
}

/*!
  Constructor
  \param r - menu reader
  \param mgr - menu manager
*/
QtxActionMenuMgr::MenuCreator::MenuCreator( QtxActionMgr::Reader* r, QtxActionMenuMgr* mgr )
: QtxActionMgr::Creator( r ),
myMgr( mgr )
{
}

/*!
  Destructor
*/
QtxActionMenuMgr::MenuCreator::~MenuCreator()
{
}

/*!
  Appends new menu items
  \param tag - tag of item
  \param subMenu - it has submenu
  \param attr - list of attributes
  \param pId - id of action corresponding to parent item
*/
int QtxActionMenuMgr::MenuCreator::append( const QString& tag, const bool subMenu,
                                           const ItemAttributes& attr, const int pId )
{
  if( !myMgr || !reader() )
    return -1;

  QString label   = reader()->option( "label",     "label"     ),
          id      = reader()->option( "id",        "id"        ),
          pos     = reader()->option( "pos",       "pos"       ),
          group   = reader()->option( "group",     "group"     ),
          tooltip = reader()->option( "tooltip",   "tooltip"   ),
          sep     = reader()->option( "separator", "separator" ),
          accel   = reader()->option( "accel",     "accel"     ),
          icon    = reader()->option( "icon",      "icon"      ),
          toggle  = reader()->option( "toggle",    "toggle"    );

  int res = -1, actId = intValue( attr, id, -1 );

  if( subMenu )
    res = myMgr->insert( strValue( attr, label ), pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  else if( tag==sep )
    res = myMgr->insert( separator(), pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  else
  {
    QIcon set;
    QPixmap pix;
    QString name = strValue( attr, icon );
    if( !name.isEmpty() && loadPixmap( name, pix ) )
      set = QIcon( pix );

    QtxAction* newAct = new QtxAction( strValue( attr, tooltip ), set,
                                       strValue( attr, label ), 
                                       QKeySequence( strValue( attr, accel ) ),
                                       myMgr );
    newAct->setToolTip( strValue( attr, tooltip ) );
    QString toggleact = strValue( attr, toggle );
    newAct->setCheckable( !toggleact.isEmpty() );
    newAct->setChecked( toggleact.toLower() == "true" );
        
    connect( newAct );
    int aid = myMgr->registerAction( newAct, actId );
    res = myMgr->insert( aid, pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  }

  return res;
}
