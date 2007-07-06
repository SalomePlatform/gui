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

#include <QEvent>
#include <QActionEvent>
#include <QApplication>

class QtxAction::ActionNotify : public QEvent
{
public:
  ActionNotify( bool add, QWidget* wid ) : QEvent( QEvent::User ), myAdd( add ), myWidget( wid ) {};
  ~ActionNotify() {};

  bool     isAdded() const { return myAdd; }
  QWidget* widget() const { return myWidget; }

private:
  bool     myAdd;
  QWidget* myWidget;
};

/*!
  \class QtxAction
  \brief Generic action class.
*/

/*!
  \brief Constructor.

  Create an action.

  \param parent parent object
  \param name action name (in terms of QObject)
  \param toggle if \c true the action is a toggle action
*/
QtxAction::QtxAction( QObject* parent, bool toggle )
: QWidgetAction( parent )
{
  setCheckable( toggle );

  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Constructor.

  Create an action.

  \param text tooltip text
  \param icon iconset
  \param menuText menu text
  \param accel shortcut key sequence
  \param parent parent object
  \param name action name (in terms of QObject)
  \param toggle if \c true the action is a toggle action
*/
QtxAction::QtxAction( const QString& text, const QIcon& icon,
                      const QString& menuText, int accel, QObject* parent, bool toggle )
: QWidgetAction( parent )
{
  setIcon( icon );
  setText( menuText );
  setToolTip( text );
  setShortcut( accel );
  setCheckable( toggle );

  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Constructor.

  Create an action.

  \param text tooltip text
  \param menuText menu text
  \param accel shortcut key sequence
  \param parent parent object
  \param name action name (in terms of QObject)
  \param toggle if \c true the action is a toggle action
*/
QtxAction::QtxAction( const QString& text, const QString& menuText,
                      int accel, QObject* parent, bool toggle )
: QWidgetAction( parent )
{
  setText( menuText );
  setToolTip( text );
  setShortcut( accel );
  setCheckable( toggle );

  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Destructor.
  
  Does nothing for the moment.
*/
QtxAction::~QtxAction()
{
}

/*!
  \brief Event filter.
  
  Redefined from QObject. Calls virtual methods when the action is added to 
  the widget or removed from it in order to perform custom processing.
  \param o object
  \param e event
  \return default implementation
*/
bool QtxAction::eventFilter( QObject* o, QEvent* e )
{
  if ( o && o->isWidgetType() )
  {
    if ( e->type() == QEvent::ActionAdded && ((QActionEvent*)e)->action() == this )
      QApplication::postEvent( this, new ActionNotify( true, (QWidget*)o ) );
    if ( e->type() == QEvent::ActionRemoved && ((QActionEvent*)e)->action() == this )
      QApplication::postEvent( this, new ActionNotify( false, (QWidget*)o ) );
  }
  return QWidgetAction::eventFilter( o, e );
}

/*!
  \brief Called when the action is added to the widget.

  Base implementation does nothing. Can be redefined in the successor
  class to customize the behavior.

  \param w widget (menu or toolbar)
*/
void QtxAction::addedTo( QWidget* )
{
}

/*!
  \brief Called when the action is removed from the widget.

  Base implementation does nothing. Can be redefined in the successor
  class to customize the behavior.

  \param w widget (menu or toolbar)
*/
void QtxAction::removedFrom( QWidget* )
{
}

void QtxAction::customEvent( QEvent* e )
{
  ActionNotify* ae = (ActionNotify*)e;
  if ( ae->isAdded() )
    addedTo( ae->widget() );
  else
    removedFrom( ae->widget() );
}
