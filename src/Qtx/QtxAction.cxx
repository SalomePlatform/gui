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
// File:      QtxAction.cxx
// Author:    Sergey TELKOV

#include "QtxAction.h"

#include <QtGui/qmenu.h>
#include <QtGui/qmenubar.h>

/*!
	Name: QtxAction [public]
	Desc: Constructs an action with given parent and name. If toggle is true the
		  action will be a toggle action, otherwise it will be a command action.
*/

QtxAction::QtxAction( QObject* parent, const char* name, bool toggle )
: QAction( parent )
{
  setCheckable( toggle );
}

/*!
	Name: QtxAction [public]
	Desc: This constructor creates an action with the following properties: the
		  description text, the icon or iconset icon, the menu text and keyboard
		  accelerator. It is a child of given parent and named specified name.
		  If toggle is true the action will be a toggle action, otherwise it will
		  be a command action.
*/

QtxAction::QtxAction( const QString& text, const QIcon& icon,
                      const QString& menuText, int accel,
                      QObject* parent, const char* name, bool toggle )
: QAction( icon, menuText, parent )
{
  setToolTip( text );
  setShortcut( accel );
  setCheckable( toggle );
}

/*!
	Name: QtxAction [public]
	Desc: This constructor creates an action with the following properties: the
		  description text, the menu text and keyboard accelerator. It is a child
		  of given parent and named specified name. If toggle is true the action
		  will be a toggle action, otherwise it will be a command action.
*/

QtxAction::QtxAction( const QString& text, const QString& menuText, int accel,
                      QObject* parent, const char* name, bool toggle )
: QAction( menuText, parent )
{
  setToolTip( text );
  setShortcut( accel );
  setCheckable( toggle );
}

/*!
	Name: ~QtxAction [virtual public]
	Desc: Destructor.
*/

QtxAction::~QtxAction()
{
}

/*!
	Name: addTo [virtual public]
	Desc: Adds this action to widget. Returns true if the action was added
		  successfully and false otherwise.
*/

bool QtxAction::addTo( QWidget* w )
{
  if ( !w )
    return false;

  w->addAction( this );
  return true;
}

/*!
	Name: addTo [virtual public]
	Desc: Adds this action to widget. If widget is QPopupMenu given index will
		  be used for menu item inserting. Returns true if the action was added
		  successfully and false otherwise.
*/

bool QtxAction::addTo( QWidget* w, const int index )
{
  if ( !w )
    return false;

  QAction* b = 0;
  if ( 0 <= index && index < w->actions().count() )
    b = w->actions().at( index );

  w->insertAction( b, this );

  return true;
}

/*!
	Name: removeFrom [virtual public]
	Desc: Removes this action from widget. Returns true if the action was removed
		  successfully and false otherwise.
*/

bool QtxAction::removeFrom( QWidget* w )
{
  if ( !w )
    return false;

  w->removeAction( this );
  return true;
}

/*!
	Name: setPopup [virtual public]
	Desc: Set or unset the sub popup menu for item with specified id in the given popup.
*/

void QtxAction::setPopup( QWidget* w, const int id, QMenu* subPopup ) const
{
/*
  if ( !w )
    return;

  QMenuData* pmd = 0;

  if ( w->inherits( "QPopupMenu" ) )
    pmd = ::qt_cast<QPopupMenu*>( w );
  else if ( w->inherits( "QMenuBar" ) )
    pmd = ::qt_cast<QMenuBar*>( w );

  if ( !pmd )
    return;  // bad widget

  QMenuData* md = 0;
  QMenuItem* item = pmd->findItem( id, &md );
  if ( !item || md != pmd )
    return;  // item is not found

  QPopupMenu* oldPopup = item->popup();
  if ( oldPopup == subPopup )
    return;  // popup is not changed

  // get properties
  int accel = pmd->accel( id );
  bool isOn = pmd->isItemEnabled( id );
  bool isVisible = pmd->isItemVisible( id );
  int pos = pmd->indexOf( id );
  QString text = pmd->text( id );
  QIconSet icon;
  if ( pmd->iconSet( id ) )
    icon = *pmd->iconSet( id );

  // remove previous item
  pmd->removeItem( id );

  // add new item
  if ( w->inherits( "QPopupMenu" ) )
  {
    // --- QPopupMenu ---
    QPopupMenu* popup = (QPopupMenu*)w;
    if ( icon.isNull() )
      pos = popup->indexOf( subPopup ? popup->insertItem( text, subPopup, id, pos ) :
			                                 popup->insertItem( text, id, pos ) );
    else
      pos = popup->indexOf( subPopup ? popup->insertItem( icon, text, subPopup, id, pos ) : 
			                                 popup->insertItem( icon, text, id, pos ) );
  }
  else
  {
    // --- QMenuBar ---
    QMenuBar* mb = (QMenuBar*)w;
    if ( icon.isNull() )
      pos = mb->indexOf( subPopup ? mb->insertItem( text, subPopup, id, pos ) : 
 			                              mb->insertItem( text, id, pos ) );
    else
      pos = mb->indexOf( subPopup ? mb->insertItem( icon, text, subPopup, id, pos ) : 
 			                              mb->insertItem( icon, text, id, pos ) );
  }

  // restore properties
  pmd->setId( pos, id ); // for sure (if id < 0)
  pmd->setAccel( accel, id );
  pmd->setItemEnabled( id, isOn );
  pmd->setItemVisible( id, isVisible );

  // delete old popup
  delete oldPopup;
*/
}
