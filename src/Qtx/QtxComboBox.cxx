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
// File:      QtxComboBox.cxx
// Author:    Sergey TELKOV

#include "QtxComboBox.h"

#include <QtCore/qlist.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qlineedit.h>

/*!
  Constructor
*/
QtxComboBox::QtxComboBox( QWidget* parent )
: QComboBox( parent ),
myCleared( false )
{
    connect( this, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
    connect( this, SIGNAL( activated( const QString& ) ), this, SLOT( onActivated( const QString& ) ) );
}

/*!
  Destructor
*/
QtxComboBox::~QtxComboBox()
{
}

/*!
  \return true if combobox is cleared
*/
bool QtxComboBox::isCleared() const
{
  return myCleared;
}

/*!
  Sets cleared status
  \param isClear - new status
*/
void QtxComboBox::setCleared( const bool isClear )
{
  if ( myCleared == isClear )
    return;
    
  myCleared = isClear;
    
  if ( isEditable() )
  {
    if ( myCleared )
      lineEdit()->setText( "" );
    else
      lineEdit()->setText( itemText( currentIndex() ) );
  }
    
  update();
}

/*!
  Sets currently selected item
  \param idx - index of item
*/
void QtxComboBox::setCurrentIndex( int idx )
{
  if ( idx < 0 || idx >= count() )
    return;
    
  myCleared = false;
  QComboBox::setCurrentIndex( idx );
}

/*!
  \return current selected id
*/
int QtxComboBox::currentId() const
{
  return id( currentIndex() );
}

/*!
  Sets current selected id
*/
void QtxComboBox::setCurrentId( int num )
{
  setCurrentIndex( index( num ) );
}

/*!
  Custom paint event handler
*/
void QtxComboBox::paintEvent( QPaintEvent* e )
{
  if ( !count() || !myCleared || isEditable() )
    QComboBox::paintEvent( e );
  else
    paintClear( e );
}

/*!
  SLOT: called if some item is activated
  \param idx - index of activated item
*/
void QtxComboBox::onActivated( int idx )
{
  resetClear();
}

/*!
  SLOT: called if some item is activated
*/void QtxComboBox::onActivated( const QString& )
{
    resetClear();
}

/*!
  Strips "cleared" state and updates
*/
void QtxComboBox::resetClear()
{
    if ( !myCleared )
        return;
    
    myCleared = false;
    update();
}

/*!
  Draws combobox when it is cleared or isn't editable
*/
void QtxComboBox::paintClear( QPaintEvent* e )
{
  int curIndex = currentIndex();
  QString curText = itemText( curIndex );
  QIcon curIcon = itemIcon( curIndex );
    
  bool upd = updatesEnabled();
  setUpdatesEnabled( false );
    
  setItemIcon( curIndex, QIcon() );
  setItemText( curIndex, QString::null );

  QComboBox::paintEvent( e );
    
  setItemText( curIndex, curText );
  setItemIcon( curIndex, curIcon );
    
  setUpdatesEnabled( upd );
}

/*!
  \return id by index
*/
int QtxComboBox::id( const int idx ) const
{
  int id = -1;
  if ( myIndexId.contains( idx ) )
    id = myIndexId[idx];
  return id;
}

/*!
  \return index by id
*/
int QtxComboBox::index( const int id ) const
{
  int idx = -1;
  for ( IndexIdMap::ConstIterator it = myIndexId.begin(); it != myIndexId.end() && idx == -1; ++it )
  {
    if ( it.value() == id )
      idx = it.key();
  }
  return idx;
}
