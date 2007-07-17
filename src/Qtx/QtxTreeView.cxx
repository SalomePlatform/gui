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
// File:      QtxTreeView.cxx
// Author:    Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "QtxTreeView.h"

#include <QHeaderView>
#include <QMenu>
#include <QMouseEvent>

/*!
  \class QtxTreeView::Header
  \brief Custom tree view header class.
  \internal
*/

class QtxTreeView::Header : public QHeaderView
{
public:
  Header( QWidget* parent = 0 );
  ~Header();

protected:
  void contextMenuEvent( QContextMenuEvent* );
};

/*!
  \brief Constructor
  \param parent parent widget
  \internal
*/
QtxTreeView::Header::Header( QWidget* parent )
: QHeaderView( Qt::Horizontal, parent )
{
}

/*!
  \brief Destructor
  \internal
*/
QtxTreeView::Header::~Header()
{
}

/*!
  \brief Customize context menu event.
  \internal

  Shows popup menu with the list of the available columns allowing the user to
  show/hide the specified column.

  \param e context menu event
*/
void QtxTreeView::Header::contextMenuEvent( QContextMenuEvent* e )
{
  QMenu menu;
  QMap<QAction*, int> actionMap;
  for ( int i = 0; i < count(); i++ ) {
    QString  lab         = model()->headerData( i, orientation(), Qt::DisplayRole ).toString();
    QVariant iconData    = model()->headerData( i, orientation(), Qt::DecorationRole );
    QVariant appropriate = model()->headerData( i, orientation(), Qtx::AppropriateRole );
    QIcon icon;
    if ( iconData.isValid() ) {
      if ( qVariantCanConvert<QIcon>( iconData ) )
	icon = qVariantValue<QIcon>( iconData );
      else if ( qVariantCanConvert<QPixmap>( iconData ) )
	icon = qVariantValue<QPixmap>( iconData );
    }
    if ( ( !lab.isEmpty() || !icon.isNull() ) && 
	 appropriate.isValid() ? appropriate.toBool() : true ) {
      QAction* a = menu.addAction( icon, lab );
      a->setCheckable( true );
      a->setChecked( !isSectionHidden( i ) );
      actionMap.insert( a, i );
    }
  }
  if ( !menu.isEmpty() ) {
    QAction* a = menu.exec( e->globalPos() );
    if ( a && actionMap.contains( a ) ) 
      setSectionHidden( actionMap[ a ], !isSectionHidden( actionMap[ a ] ) );
  }
  e->accept();
}

/*!
  \class QtxTreeView
  \brief Tree view class with possibility to display columns popup menu.

  The QtxTreeView class represents a customized tree view class. In addition to the
  base functionality inherited from the QTreeView class, clicking at the tree view 
  header with the right mouse button displays the popup menu allowing the user
  to show/hide specified columns.

  By default the popup menu contains items corresponding to all the tree view columns.
  In order to disable some columns from being shown in the popup menu one may customize
  the data model (see QAbstractItemModel class). The custom model should implement
  headerData() method and return \c true for the Qtx::AppropriateRole role for
  those columns which should be available in the popup menu and \c false for the columns
  which should not be added to it. 
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxTreeView::QtxTreeView( QWidget* parent )
: QTreeView( parent )
{
  setHeader( new Header() );
}

/*!
  \brief Destructor.
*/
QtxTreeView::~QtxTreeView()
{
}
