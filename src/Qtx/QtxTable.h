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
// File:      QtxTable.h
// Author:    Sergey TELKOV

#ifndef QTXTABLE_H
#define QTXTABLE_H

#include "Qtx.h"
#include "QtxTableInterface.h"

#include <QMap>
#include <QVector>
#include <QTableWidget>
#include <QHeaderView>
#include <QIcon>
#include <QModelIndex>
//#include <qvariant.h>
//#include <qptrvector.h>

#ifndef QT_NO_TABLE

class QHeaderView;
class QToolButton;
class QVariant;
class QtxStyleWrap;

#ifdef WIN32
#pragma warning( disable : 4251 )
#endif

class QTX_EXPORT QtxTable : public QTableWidget, public QtxTableInterface
{
  Q_OBJECT

  //Q_PROPERTY( bool selectAllEnabled READ isSelectAllEnabled WRITE setSelectAllEnabled )

  class Header;
  class StyleItem;

public:
  QtxTable( QWidget* = 0, const char* = 0 );
  QtxTable( int, int, QWidget* = 0, const char* = 0 );
  virtual ~QtxTable();

  virtual int      rowCount() const;
  virtual int      columnCount() const;

  bool             headerEditable( Qt::Orientation, const int = -1 ) const;

  bool             editHeader( Qt::Orientation, const int );
  void             endEditHeader( const bool = true );

  virtual void     setSelectionMode( SelectionMode );

  void             removeSelected( const bool row = true );

  virtual bool     eventFilter( QObject*, QEvent* );

  //virtual void     setNumRows( int );
  //virtual void     setNumCols( int );

  bool             isSelectAllEnabled() const;
  virtual void     setSelectAllEnabled( const bool );

  QHeaderView*     header( const Qt::Orientation, const int ) const;
  int              numHeaders( const Qt::Orientation ) const;
  void             setNumHeaders( const Qt::Orientation, const int );

  virtual QVariant headerData( const int, const Qt::Orientation,
                               const int = Qt::DisplayRole ) const;
  virtual QFont    headerFont( const Qt::Orientation, const int ) const;
  virtual QColor   headerForeground( const Qt::Orientation, const int ) const;
  virtual QColor   headerBackground( const Qt::Orientation, const int ) const;
  virtual QIcon    headerIcon( const Qt::Orientation, const int ) const;

  virtual void     setHeaderData( const int, const Qt::Orientation, const QVariant&,
                                  const int = Qt::DisplayRole );
  virtual void     setHeaderFont( const Qt::Orientation, const int, const QFont& );
  virtual void     setHeaderForeground( const Qt::Orientation, const int, const QColor& );
  virtual void     setHeaderBackground( const Qt::Orientation, const int, const QColor& );
  virtual void     setHeaderIcon( const Qt::Orientation, const int, const QIcon& );

  virtual QVariant data( const int theRow, const int theColumn,
                         const int theRole = Qt::DisplayRole ) const;
  virtual QVariant cellData( const int, const int ) const;
  virtual QFont    cellFont( const int, const int ) const;
  virtual QColor   cellForeground( const int, const int ) const;
  virtual QColor   cellBackground( const int, const int ) const;
  virtual QIcon    cellIcon( const int, const int ) const;

  virtual void     setData( const int theRow, const int theColumn,
                            const QVariant& theValue,
                            const int theRole = Qt::DisplayRole );
  virtual void     setCellData( const int, const int, const QVariant& );
  virtual void     setCellFont( const int, const int, const QFont& );
  virtual void     setCellForeground( const int, const int, const QColor& );
  virtual void     setCellBackground( const int, const int, const QColor& );
  virtual void     setCellIcon( const int, const int, QIcon& );

  virtual QTableWidgetItem* getItem( const int, const int, const bool = true );

  virtual QItemSelectionModel* selectionModel() const;
  virtual QModelIndexList getSelectedIndexes() const;

  bool             indexPosition( const QModelIndex&, int&, int& ) const;

  //virtual void     paintCell( QPainter*, int, int, const QRect&, bool, const QColorGroup& );

  virtual void     clear( const bool = true );

signals:
  void             headerEdited( QHeaderView*, int );
  void             headerEdited( Qt::Orientation, int );

public slots:
  virtual void     selectAll();
  virtual void     setTopMargin( int );
  virtual void     setLeftMargin( int );
  void             setHeadersEditable( Qt::Orientation, bool );
  virtual void     setHeaderEditable( Qt::Orientation, bool, const int = -1 );

  //virtual void     insertRows( int, int = 1 );
  //virtual void     insertColumns( int, int = 1 );
  //virtual void     removeRow( int );
  //virtual void     removeRows( const QVector<int>& );
  //virtual void     removeColumn( int );
  //virtual void     removeColumns( const QVector<int>& );

  virtual void     setUpdatesEnabled( bool enable );

  virtual void     adjustRow( int );
  virtual void     adjustColumn( int );

protected slots:
  virtual void     rowHeightChanged( int );
  virtual void     columnWidthChanged( int );

private slots:
  void             onScrollBarMoved( int );
  void             onHeaderSizeChange( int, int, int );

protected:
  virtual void     hideEvent( QHideEvent* );
  virtual void     resizeEvent( QResizeEvent* );

  virtual bool     beginHeaderEdit( QHeaderView*, const int );
  bool             beginHeaderEdit( Qt::Orientation, const int, const int = -1 );
  virtual void     endHeaderEdit( const bool = true );
  bool             isHeaderEditing() const;
  virtual QWidget* createHeaderEditor( QHeaderView*, const int, const bool = true );
  virtual void     setHeaderContentFromEditor( QHeaderView*, const int, QWidget* );

  QHeaderView*     header( Qt::Orientation o ) const;
  virtual QRect    headerSectionRect( QHeaderView*, const int, int* = 0 ) const;

private:
  typedef QMap<QHeaderView*, bool>  HeaderState;
  typedef QVector<QHeaderView*> HeaderVector;

private:
  void             updateHeaderEditor();
  bool             beginHeaderEdit( QHeaderView*, const QPoint& );

  QtxStyleWrap*    styleWrapper();
  HeaderVector*    headerVector( const Qt::Orientation ) const;

  //void             updateGeometries();
  void             updateSelectAllButton();

  void             updateHeaders( const Qt::Orientation );
  void             updateHeaderSizes( const Qt::Orientation );
  void             updateHeaderSpace( const Qt::Orientation );
  void             updateHeaderGeometries( const Qt::Orientation );

private:
  /*
  HeaderVector     myVerHeaders;
  HeaderVector     myHorHeaders;
  QtxStyleWrap*    myStyleWrapper;

  QWidget*         myHeaderEditor;
  QHeaderView*     myEditedHeader;
  int              myEditedSection;
  HeaderState      myHeaderEditable;

  QToolButton*     mySelectAll;
  */

  friend class QtxTable::Header;
};

#ifdef WIN32
#pragma warning( default: 4251 )
#endif

#endif

#endif // QTXTABLE_H
