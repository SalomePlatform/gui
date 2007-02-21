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
#include <QtGui/qevent.h>
#include <QtGui/qmenubar.h>
#include <QtGui/qapplication.h>

/*!
	Name: QtxAction [public]
	Desc: Constructs an action with given parent and name. If toggle is true the
		  action will be a toggle action, otherwise it will be a command action.
*/

QtxAction::QtxAction( QObject* parent, const char* name, bool toggle )
: QAction( parent )
{
  setCheckable( toggle );

  QApplication::instance()->installEventFilter( this );
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

  QApplication::instance()->installEventFilter( this );
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

  QApplication::instance()->installEventFilter( this );
}

/*!
	Name: ~QtxAction [virtual public]
	Desc: Destructor.
*/

QtxAction::~QtxAction()
{
}

bool QtxAction::eventFilter( QObject* o, QEvent* e )
{
  if ( o && o->isWidgetType() )
  {
    if ( e->type() == QEvent::ActionAdded && ((QActionEvent*)e)->action() == this )
      addedTo( (QWidget*)o );
    if ( e->type() == QEvent::ActionRemoved && ((QActionEvent*)e)->action() == this )
      removedFrom( (QWidget*)o );
  }
  return QAction::eventFilter( o, e );
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

void QtxAction::addedTo( QWidget* )
{
}

void QtxAction::removedFrom( QWidget* )
{
}
