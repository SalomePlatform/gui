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
	Name: QtxMRUAction [public]
	Desc: Constructs an MRU action with given parent and name.
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
	Name: QtxMRUAction [public]
	Desc: This constructor creates an action with the following properties: the
		    description text, the menu text and.  It is a child of given parent and
        named specified name.
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
	Name: QtxMRUAction [public]
	Desc: This constructor creates an action with the following properties: the
		    description text, the menu text, the icon or iconset icon and keyboard
        accelerator. It is a child of given parent and named specified name.
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
	Name: ~QtxMRUAction [public]
	Desc: This destructor removes all added popup items.
*/

QtxMRUAction::~QtxMRUAction()
{
  delete menu();
}

/*!
	Name: insertMode [public]
	Desc: Returns the insert mode.
*/

int QtxMRUAction::insertMode() const
{
  return myInsertMode;
}

/*!
	Name: setInsertMode [public]
	Desc: Returns the insert mode. Can be following values:
      MoveFirst - place the specified link to the first position in any case
      MoveLast  - place the specified link to the last position in any case
      AddFirst  - if inserted link doesn't exist then add to the first position
      AddLast   - if inserted link doesn't exist then add to the lase position
*/

void QtxMRUAction::setInsertMode( const int mode )
{
  myInsertMode = mode;
}

/*!
	Name: count [public]
	Desc: Returns the number of links.
*/

int QtxMRUAction::count() const
{
  return myLinks.count();
}

/*!
	Name: isEmpty [public]
	Desc: Returns 'true' if there is no links.
*/

bool QtxMRUAction::isEmpty() const
{
  return myLinks.isEmpty();
}

/*!
	Name: visibleCount [public]
	Desc: Returns the number of first links which will be added to popup menu.
        If 'visibleCount' less than 1 then all links will be used.
*/

int QtxMRUAction::visibleCount() const
{
  return myVisCount;
}

/*!
	Name: setVisibleCount [public]
	Desc: Sets the number of links which will be used in popup menu.
*/

void QtxMRUAction::setVisibleCount( int num )
{
  if ( myVisCount == num )
    return;

  myVisCount = num;
}

/*!
	Name: insert [public]
	Desc: Insert the link according to the insert mode.
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
	Name: remove [public]
	Desc: Removes link with specified index.
*/

void QtxMRUAction::remove( const int idx )
{
  if ( idx < 0 || idx >= (int)myLinks.count() )
    return;

  myLinks.removeAt( idx );
}

/*!
	Name: remove [public]
	Desc: Removes specified link.
*/

void QtxMRUAction::remove( const QString& link )
{
  myLinks.removeAll( link );
}

/*!
	Name: item [public]
	Desc: Returns the link with specified index.
*/

QString QtxMRUAction::item( const int idx ) const
{
  QString res;
  if ( idx >= 0 && idx < (int)myLinks.count() )
    res = myLinks[idx];
  return res;
}

/*!
	Name: find [public]
	Desc: Find specified link. If link exists then returns index otherwise -1 returned.
*/

int QtxMRUAction::find( const QString& link ) const
{
  return myLinks.indexOf( link );
}

/*!
	Name: contains [public]
	Desc: Returns 'true' if given link exist.
*/

bool QtxMRUAction::contains( const QString& link ) const
{
  return myLinks.contains( link );
}

/*!
	Name: loadLinks [public]
	Desc: Load the MRU links from specified resource manager section.
        If parameter 'clear' is 'true' then link list will be cleared first.
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
	Name: saveLinks [public]
	Desc: Save the MRU links into specified resource manager section.
        If parameter 'clear' is 'true' then section will be cleared first.
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
	Name: onAboutToShow [private slots]
	Desc: Enable or disable sub menu item according to number of MRU links
        in sub popup when parent popup is shown.
*/

void QtxMRUAction::onAboutToShow()
{
  updateMenu();
}

/*!
	Name: onActivated [private slot]
	Desc: Process popup item activation and emit signal activated with selected MRU link.
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
	Name: updateMenu [private]
	Desc: Updates the popup menu which contains MRU link items.
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
