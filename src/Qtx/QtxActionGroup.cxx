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
// File:      QtxActionGroup.cxx
// Author:    Sergey TELKOV

#include "QtxActionGroup.h"

#include "QtxComboBox.h"

#include <QApplication>

/*!
  \class QtxActionGroup
  \brief An action class which is represented in the menu bar (or toolbar) as
  a group of actions or combo box and support exclusive triggerd action behaviour.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
QtxActionGroup::QtxActionGroup( QObject* parent )
: QtxActionSet( parent ),
  myExclusive( false ),
  myDropDown( false )
{
  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( onTriggered( QAction* ) ) );
}

/*!
  \brief Destructor.
*/
QtxActionGroup::~QtxActionGroup()
{
}

/*!
  \brief Returns true if the action group is exclusive.
*/
bool QtxActionGroup::isExclusive() const
{
  return myExclusive;
}

/*!
  \brief Set the action group is exclusive.
  \param on if true group should be exclusive otherwise not.
*/
void QtxActionGroup::setExclusive( const bool on )
{
  if ( myExclusive == on )
    return;

  bool e = isEmptyAction();

  myExclusive = on;

  if ( e != isEmptyAction() )
    updateType();
}

/*!
  \brief Returns .
  \param on if true group should be exclusive otherwise not.
*/
bool QtxActionGroup::usesDropDown() const
{
  return myDropDown;
}

/*!
  \brief Sets whether the group's actions are displayed in a subwidget of the widgets the action group is added to.
*/
void QtxActionGroup::setUsesDropDown( const bool on )
{
  if ( myDropDown == on )
    return;

  bool e = isEmptyAction();

  myDropDown = on;

  if ( e != isEmptyAction() )
    updateType();
}

/*!
  \brief Append the specified action into group.
*/
void QtxActionGroup::add( QAction* a )
{
  insertAction( a );
}

/*!
  \brief Called when some action is activated by the user.
  \param a toggled action
*/
void QtxActionGroup::onTriggered( QAction* a )
{
  if ( !isExclusive() )
    return;

  if ( !a->isCheckable() || !a->isChecked() )
    return;

  QList<QAction*> aList = actions();
  for ( QList<QAction*>::const_iterator it = aList.begin(); it != aList.end(); ++it )
  {
    QAction* cur = *it;
    if ( cur->isCheckable() )
      cur->setChecked( cur == a );
  }

  emit selected( a );
}

void QtxActionGroup::onActivated( int id )
{
  const QObject* s = sender();

  QAction* a = action( id );
  if ( !a )
    return;

  if ( a->isChecked() )
    return;

  a->setChecked( true );

  QList<QWidget*> lst = createdWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QtxComboBox* cb = ::qobject_cast<QtxComboBox*>( *it );
    if ( cb && cb != s )
      cb->setCurrentId( id );
  }
}

/*!
  \brief Update action set for the specified widget.
  \param w a widget this action is added to
*/
void QtxActionGroup::updateAction( QWidget* w )
{
  QtxComboBox* cb = createdWidget( w );
  if ( !cb )
    QtxActionSet::updateAction( w );
  else
  {
    updateAction( cb );

    QList<QAction*> lst = actions();
    for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
      w->removeAction( *it );
  }
}

void QtxActionGroup::updateAction( QtxComboBox* cb )
{
  if ( !cb )
    return;

  cb->clear();
  cb->setCleared( false );

  QAction* cur = 0;
  QList<QAction*> lst = actions();
  for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QAction* a = *it;
    cb->addItem( a->icon(), a->text() );
    cb->setId( cb->count() - 1, actionId( a ) );
    if ( a->isChecked() )
      cur = a;
  }

  if ( cur )
    cb->setCurrentId( actionId( cur ) );
  else
    cb->setCleared( true );
}

QWidget* QtxActionGroup::createWidget( QWidget* p )
{
  QtxComboBox* cb = !isEmptyAction() ? new QtxComboBox( p ) : 0;
  if ( cb )
    connect( cb, SIGNAL( activatedId( int ) ), this, SLOT( onActivated( int ) ) );
  return cb;
}

/*!
  \brief Check if the action itself should be invisible
  (only child action are shown)
  \return \c true if the action itself should be visible
*/
bool QtxActionGroup::isEmptyAction() const
{
  return !isExclusive() || !usesDropDown();
}


void QtxActionGroup::updateType()
{
  QList<QWidget*> lst = associatedWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QWidget* w = *it;
    QList<QAction*> lst = w->actions();

    int i = lst.indexOf( this );
    w->removeAction( this );

    lst = w->actions();
    w->insertAction( i < lst.count() ? lst.at( i ) : 0, this );
  }
}

QtxComboBox* QtxActionGroup::createdWidget( QWidget* p )
{
  QtxComboBox* cb = 0;
  QList<QWidget*> lst = createdWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end() && !cb; ++it )
  {
    if ( (*it)->parent() == p )
      cb = ::qobject_cast<QtxComboBox*>( *it );
  }
  return cb;
}





/*!
  \fn void QtxActionGroup::selected( QAction* a );
  \brief Emitted when some child action is checked by the user.
  \param a action being checked
*/
