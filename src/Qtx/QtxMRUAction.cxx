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
// File:      QtxMRUAction.cxx
// Author:    Sergey TELKOV

#include "QtxMRUAction.h"

#include "QtxResourceMgr.h"

#include <QMenu>
#include <QIcon>

/*!
  \class QtxMRUAction
  \brief Menu action which provides most recent used items support.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
QtxMRUAction::QtxMRUAction( QObject* parent )
: QtxAction( "Most Recently Used", "Most Recently Used", 0, parent ),
  myVisCount( 5 ),
  myInsertMode( MoveFirst )
{
  setMenu( new QMenu( 0 ) );
  connect( menu(), SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Constructor.
  \param description (tooltip) text
  \param menuText menu text
  \param parent parent object
*/
QtxMRUAction::QtxMRUAction( const QString& text, const QString& menuText, QObject* parent )
: QtxAction( text, menuText, 0, parent ),
  myVisCount( 5 ),
  myInsertMode( MoveFirst )
{
  setMenu( new QMenu( 0 ) );
  connect( menu(), SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Constructor.
  \param description (tooltip) text
  \param icon action icon
  \param menuText menu text
  \param parent parent object
*/
QtxMRUAction::QtxMRUAction( const QString& text, const QIcon& icon,
                            const QString& menuText, QObject* parent )
: QtxAction( text, icon, menuText, 0, parent ),
myVisCount( 5 ),
myInsertMode( MoveFirst )
{
  setMenu( new QMenu( 0 ) );
  connect( menu(), SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Destructor.
*/
QtxMRUAction::~QtxMRUAction()
{
  delete menu();
}

/*!
  \brief Get items insertion policy.
  \return insertion policy (QtxMRUAction::InsertionMode)
*/
int QtxMRUAction::insertMode() const
{
  return myInsertMode;
}

/*!
  \brief Set items insertion policy.
  \param mode insertion policy (QtxMRUAction::InsertionMode)
*/
void QtxMRUAction::setInsertMode( const int mode )
{
  myInsertMode = mode;
}

/*!
  \brief Get number of MRU items.
  \return number of MRU items
*/
int QtxMRUAction::count() const
{
  return myLinks.count();
}

/*!
  \brief Check if the MRU items list is empty.
  \return \c true if there are no MRU items
*/
bool QtxMRUAction::isEmpty() const
{
  return myLinks.isEmpty();
}

/*!
  \brief Get number of visible MRU items.
  \return visible MRU items number
  \sa setVisibleCount()
*/
int QtxMRUAction::visibleCount() const
{
  return myVisCount;
}

/*!
  \brief Set number of visible MRU items.
  
  This method sets the maximum number of MRU items
  to be displayed in the popup menu (5 by default).

  If \a num < 1, then all MRU items will be displayed.

  \param num visible MRU items number
*/
void QtxMRUAction::setVisibleCount( int num )
{
  if ( myVisCount == num )
    return;

  myVisCount = num;
}

/*!
  \brief Insert MRU item.

  The item is inserted according to the current insertion policy.

  \param link MRU item to be added
*/
void QtxMRUAction::insert( const QString& link )
{
  if ( myLinks.contains( link ) && ( insertMode() == AddFirst || insertMode() == AddLast ) )
    return;

  myLinks.removeAll( link );

  switch ( insertMode() )
  {
  case AddFirst:
  case MoveFirst:
    myLinks.prepend( link );
    break;
  case AddLast:
  case MoveLast:
    myLinks.append( link );
    break;
  }
}

/*!
  \brief Remove MRU item.

  Does nothing if \a idx is out of range.

  \param idx MRU item index
*/
void QtxMRUAction::remove( const int idx )
{
  if ( idx < 0 || idx >= (int)myLinks.count() )
    return;

  myLinks.removeAt( idx );
}

/*!
  \brief Remove MRU item.

  Does nothing if there is no speicified item in the list.

  \param link MRU item to be removed
*/
void QtxMRUAction::remove( const QString& link )
{
  myLinks.removeAll( link );
}

/*!
  \brief Get MRU item
  \param idx MRU item index
  \return MRU item or null QString if \a idx is out of range
*/
QString QtxMRUAction::item( const int idx ) const
{
  QString res;
  if ( idx >= 0 && idx < (int)myLinks.count() )
    res = myLinks[idx];
  return res;
}

/*!
  \brief Get MRU item index.
  \param link MRU item
  \return MRU item index or -1 if item is not found
*/
int QtxMRUAction::find( const QString& link ) const
{
  return myLinks.indexOf( link );
}

/*!
  \brief Check if MRU item is in the list.
  \param link MRU item
  \return \c true if specified item is already added to the list
*/
bool QtxMRUAction::contains( const QString& link ) const
{
  return myLinks.contains( link );
}

/*!
  \brief Load the MRU items from specified resources section.
  \param resMgr resources manager
  \param section resources section
  \param clear if \c true, previous MRU items list is cleared
*/
void QtxMRUAction::loadLinks( QtxResourceMgr* resMgr, const QString& section, const bool clear )
{
  if ( !resMgr || section.isEmpty() )
    return;

  if ( clear )
    myLinks.clear();

  QString itemPrefix( "item_" );

  QMap<QString, int> map;
  for ( QStringList::const_iterator itr = myLinks.begin(); itr != myLinks.end(); ++ itr )
    map.insert( *itr, 0 );

  QStringList items = resMgr->parameters( section );
  for ( QStringList::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( !(*it).startsWith( itemPrefix ) )
      continue;

    QString link = resMgr->stringValue( section, *it, QString::null );
    if ( link.isEmpty() || map.contains( link ) )
      continue;

    myLinks.append( link );
    map.insert( link, 0 );
  }
}

/*!
  \brief Save the MRU items to specified resources section.
  \param resMgr resources manager
  \param section resources section
  \param clear if \c true, the resources section is first cleared
*/
void QtxMRUAction::saveLinks( QtxResourceMgr* resMgr, const QString& section, const bool clear ) const
{
  if ( !resMgr || section.isEmpty() )
    return;

  if ( clear )
    resMgr->remove( section );

  QStringList lst;
  QMap<QString, int> map;
  for ( QStringList::const_iterator itr = myLinks.begin(); itr != myLinks.end(); ++itr )
  {
    lst.append( *itr );
    map.insert( *itr, 0 );
  }

  QString itemPrefix( "item_" );
  QStringList items = resMgr->parameters( section );
  for ( QStringList::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( !(*it).startsWith( itemPrefix ) )
      continue;

    QString link = resMgr->stringValue( section, *it, QString::null );
    if ( !link.isEmpty() && !map.contains( link ) )
    {
      lst.append( link );
      map.insert( link, 0 );
    }

    resMgr->remove( section, *it );
  }

  int counter = 0;
  for ( QStringList::const_iterator iter = lst.begin(); iter != lst.end(); ++iter, counter++ )
    resMgr->setValue( section, itemPrefix + QString().sprintf( "%03d", counter ), *iter );
}

/*!
  \brief Prepare MRU items popup menu.
  
  This method is called when the parent menu is shown.
  Enables or disables sub menu item according to the number of MRU items.
*/
void QtxMRUAction::onAboutToShow()
{
  updateMenu();
}

/*!
  \brief Called when any MRU item is selected by the user.

  Emits signal activated(const QString&) passing selected MRU item as parameter.
*/
void QtxMRUAction::onActivated()
{
  QAction* a = ::qobject_cast<QAction*>( sender() );
  if ( !a )
    return;

  QString link = a->text();
  if ( !link.isEmpty() && myLinks.contains( link ) )
    emit activated( link );
}

/*!
  \brief Update MRU items popup menu.
*/
void QtxMRUAction::updateMenu()
{
  QMenu* pm = menu();
  if ( !pm )
    return;

  pm->clear();

  int count = visibleCount() < 0 ? myLinks.count() : visibleCount();
  for ( QStringList::const_iterator it = myLinks.begin(); it != myLinks.end() && count > 0; ++it, count-- )
    pm->addAction( *it, this, SLOT( onActivated() ) );
}

/*!
  \fn void QtxMRUAction::activated( const QString& link );
  \brief Emitted when user selects any MRU item in the menu.
  \param link selected MRU item
*/
