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
// File   : LightApp_WidgetContainer.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "LightApp_WidgetContainer.h"

#include <QStackedWidget>

/*!
  \class LightApp_WidgetContainer
  \brief Class which implements widget container.

  This class allows to put all the child widgets in the stack widget.
  Only one widget is visible at the moment. 

  Each widget can be referred by unique number identifier passed as parameter
  to the insert() method. Removing of the widgets from the container is done
  via remove() method.

  To raise the widget to the top (to make it visiable) use activate() method.
  Current widget can be retrieved with active() method.
*/

/*!
  \brief Constructor.
  \param type widget container type
  \param parent parent widget
*/
LightApp_WidgetContainer::LightApp_WidgetContainer( const int type, QWidget* parent )
: QDockWidget( parent ),
  myType( type )
{
  setObjectName( QString( "WC_%1" ).arg( type ) );
  setWidget( myStack = new QStackedWidget( this ) );
  myStack->show();
}

/*!
  \brief Destructor.
*/
LightApp_WidgetContainer::~LightApp_WidgetContainer()
{
}

/*!
  \brief Get the type of the widget container.
  \return widget container type (passed as parameter to the constructor)
*/
int LightApp_WidgetContainer::type() const
{
  return myType;
}

/*!
 \brief Insert widget \a wid with identifier \a id to the container.

 If there is already a widget with the specified identifier it is removed.
 
 \param id widget ID
 \param wid widget being inserted
 \return widget identifier or -1 on error
*/
int LightApp_WidgetContainer::insert( const int id, QWidget* wid )
{
  if ( id == -1 || !wid )
    return -1;

  if ( widget( id ) == wid )
    return -1;
  
  remove( id );

  myStack->addWidget( wid );
  if ( !myStack->currentWidget() )
    activate( wid );

  myMap.insert( id, wid );

  connect( wid, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  updateTitle();

  return id;
}

/*!
  \brief Remove widget with the specified identifier from the container.
  \param id widget ID
  \param del if \c true (default) destroy the child widget
*/
void LightApp_WidgetContainer::remove( const int id, const bool del )
{
  if ( myMap.contains( id ) )
    remove( myMap[ id ], del );
}

/*!
  \brief Remove specified widget from the container.
  \param wid widget being removed
  \param del if \c true (default) destroy the child widget
*/
void LightApp_WidgetContainer::remove( QWidget* wid, const bool del )
{
  if ( !wid )
    return;

  myStack->removeWidget( wid );
  QMutableMapIterator<int, QWidget*> it( myMap );
  while ( it.hasNext() ) {
    it.next();
    if ( it.value() == wid ) {
      it.remove();
      break;
    }
  }

  disconnect( wid, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  if ( del )
    delete wid;

  updateTitle();
}

/*!
  \brief Check if the container contains a widget with the 
  specified identifier.
  \param id widget ID
  \return \c true if container contains a widget
*/
bool LightApp_WidgetContainer::contains( const int id ) const
{
  return myMap.contains( id );
}

/*!
  \brief Check if the container contains specified widget.
  \param widget widget being checked
  \return \c true if container contains a widget
*/
bool LightApp_WidgetContainer::contains( QWidget* wid ) const
{
  bool found = false;
  QMapIterator<int, QWidget*> it( myMap );
  while ( it.hasNext() && !found ) {
    it.next();
    if ( it.value() == wid )
      found = true;
  }
  return found;
}

/*!
  \brief Check if the widget container is empty.
  \return \c true if widget container does not have child widgets
*/
bool LightApp_WidgetContainer::isEmpty() const
{
  return myMap.count() > 0;
}

/*!
  \brief Bring the widget with the specified identifier to the top
  of the container.
  \param id widget ID
*/
void LightApp_WidgetContainer::activate( const int id )
{
  if ( myMap.contains( id ) )
    activate( myMap[ id ] );
}

/*!
  \brief Bring the specified widget to the top of the container.
  \param wid widget to be activated
*/
void LightApp_WidgetContainer::activate( QWidget* wid )
{
  if ( !wid || !contains( wid ) )
    return;

  myStack->setCurrentWidget( wid );

  updateTitle();
}

/*!
  \brief Get widget with the specified identifier.
  \param id widget ID
  \return widget pointer or 0 if \a id is invalid
*/
QWidget* LightApp_WidgetContainer::widget( const int id ) const
{
  return myMap.contains( id ) ? myMap[ id ] : 0;
}

/*!
  \brief Get currently visible widget.
  \return current widget or 0 if there is no any
*/
QWidget* LightApp_WidgetContainer::active() const
{
  return myStack->currentWidget();
}

/*!
  \brief Customize show/hide operations.

  Emits visibilityChanged(bool) signal when the widget is 
  being shown/hidden.
  
  \param visible new visibility state
*/
void LightApp_WidgetContainer::setVisible ( bool visible )
{
  QDockWidget::setVisible( visible );

  updateTitle();

  emit( visibilityChanged( visible ) );
}

/*!
  \brief Called when the child widget is destroyed.
  \param wid widget being destroyed
*/
void LightApp_WidgetContainer::onDestroyed( QObject* wid )
{
  QMutableMapIterator<int, QWidget*> it( myMap );
  while ( it.hasNext() ) {
    it.next();
    if ( it.value() == wid ) {
      it.remove();
      break;
    }
  }
}

/*!
  \brief Update widget container title according 
  to the active child widget.
*/
void LightApp_WidgetContainer::updateTitle()
{
  setWindowTitle( myStack->currentWidget() ? myStack->currentWidget()->windowTitle() : QString() );
}
