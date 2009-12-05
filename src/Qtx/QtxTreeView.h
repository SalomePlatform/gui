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
// File:      QtxTreeView.h
// Author:    Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef QTXTREEVIEW_H
#define QTXTREEVIEW_H

#include "Qtx.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include <QTreeView>

class QTX_EXPORT QtxTreeView : public QTreeView
{
  Q_OBJECT

  class Header;

public:
  QtxTreeView( QWidget* = 0 );
  QtxTreeView( const bool, QWidget* = 0 );
  virtual ~QtxTreeView();

  void     expandLevels( const int );
  void     collapseLevels( const int );

  void     expandAll( const QModelIndex& );
  void     collapseAll( const QModelIndex& );

  void     setSortMenuEnabled( const bool );
  bool     sortMenuEnabled() const;

  void     addHeaderMenuAction( QAction* );

  void     resizeColumnToEncloseContents( int );

  void     setSortingEnabled( const bool );
  bool     sortingEnabled() const;

  void     setMultiSortEnabled( const bool );
  bool     multiSortEnabled() const;

protected slots:
  void     onHeaderClicked();
  void     rowsAboutToBeRemoved( const QModelIndex&, int, int );
  void     selectionChanged( const QItemSelection&, const QItemSelection& );
  
protected:
  void     setOpened( const QModelIndex&, const int, bool );
  virtual void drawRow( QPainter*, const QStyleOptionViewItem&,
                        const QModelIndex& ) const;
  void     drawRow( const QModelIndex& );

signals:
  void     sortingEnabled( bool );
  void     selectionChanged();
  void     drawedRow( const QModelIndex& );

private:
  void     emitSortingEnabled( bool );

  friend class QtxTreeView::Header;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // QTXTREEVIEW_H
