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
// File:   SUIT_TreeModel.h
// Author: Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef SUIT_TREEMODEL_H
#define SUIT_TREEMODEL_H

#include "SUIT.h"

#include <Qtx.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QItemDelegate>
#include <QVariant>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class SUIT_DataObject;
class SUIT_TreeModel;

class SUIT_EXPORT SUIT_TreeModel : public QAbstractItemModel
{
  Q_OBJECT

private:
  class TreeItem;
  class TreeSync;
  typedef SUIT_DataObject*          ObjPtr;
  typedef SUIT_TreeModel::TreeItem* ItemPtr;
 
public:
  //! Data role
  typedef enum {
    DisplayRole         = Qt::DisplayRole,       //!< text label
    DecorationRole      = Qt::DecorationRole,    //!< icon
    EditRole            = Qt::EditRole,          //!< edit mode
    ToolTipRole         = Qt::ToolTipRole,       //!< tooltip
    StatusTipRole       = Qt::StatusTipRole,     //!< status tip
    WhatsThisRole       = Qt::WhatsThisRole,     //!< what's this info
    FontRole            = Qt::FontRole,          //!< font
    TextAlignmentRole   = Qt::TextAlignmentRole, //!< text alignment
    BackgroundRole      = Qt::BackgroundRole,    //!< background color
    ForegroundRole      = Qt::ForegroundRole,    //!< text color
    CheckStateRole      = Qt::CheckStateRole,    //!< check state
    SizeHintRole        = Qt::SizeHintRole,      //!< size hint
    BaseColorRole       = Qt::UserRole,          //!< (editor) background color
    TextColorRole,                               //!< (editor) text color    (Qt::UserRole + 1)
    HighlightRole,                               //!< highlight color        (Qt::UserRole + 2)
    HighlightedTextRole,                         //!< highlighted text color (Qt::UserRole + 3)
    AppropriateRole     = Qtx::AppropriateRole   //!< appropriate flag       (Qt::UserRole + 100)
  } Role;

  SUIT_TreeModel( QObject* = 0 );
  SUIT_TreeModel( SUIT_DataObject*, QObject* = 0 );
  ~SUIT_TreeModel();

  SUIT_DataObject*       root() const;
  void                   setRoot( SUIT_DataObject* );

  virtual QVariant       data( const QModelIndex&, int = DisplayRole ) const;
  virtual bool           setData( const QModelIndex&, const QVariant&, int = EditRole );
  virtual Qt::ItemFlags  flags( const QModelIndex& ) const;
  virtual QVariant       headerData( int, Qt::Orientation, int = Qt::DisplayRole ) const;

  virtual QModelIndex    index( int, int, const QModelIndex& = QModelIndex() ) const;
  virtual QModelIndex    parent( const QModelIndex& ) const;

  virtual int            columnCount( const QModelIndex& = QModelIndex() ) const;
  virtual int            rowCount( const QModelIndex& = QModelIndex() ) const;

  SUIT_DataObject*       object( const QModelIndex& = QModelIndex() ) const;
  QModelIndex            index( const SUIT_DataObject*, int = 0 ) const;

  bool                   autoDeleteTree() const;
  void                   setAutoDeleteTree( const bool );

  bool                   autoUpdate() const;
  void                   setAutoUpdate( const bool, const bool = true );

  virtual bool           customSorting( const int ) const;
  virtual bool           lessThan( const QModelIndex& left, const QModelIndex& right ) const;

  QAbstractItemDelegate* delegate() const;

public slots:
  virtual void           updateTree( const QModelIndex& );
  virtual void           updateTree( SUIT_DataObject* = 0 );

private:
  void                   initialize();

  TreeItem*              rootItem() const;
  TreeItem*              treeItem( const QModelIndex& ) const;
  TreeItem*              treeItem( const SUIT_DataObject* ) const;
  SUIT_DataObject*       object( const TreeItem* ) const;

  TreeItem*              createItem( SUIT_DataObject*,
				     TreeItem* = 0, 
				     TreeItem* = 0 );
  void                   updateItem( TreeItem* );
  void                   removeItem( TreeItem* );

private slots:
  void                   onInserted( SUIT_DataObject*, SUIT_DataObject* );
  void                   onRemoved( SUIT_DataObject*, SUIT_DataObject* );

private:
  typedef QMap<SUIT_DataObject*, TreeItem*> ItemMap;
  
  SUIT_DataObject*       myRoot;
  TreeItem*              myRootItem;
  ItemMap                myItems;
  bool                   myAutoDeleteTree;
  bool                   myAutoUpdate;

  friend class SUIT_TreeModel::TreeSync;
};

class SUIT_EXPORT SUIT_ProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  SUIT_ProxyModel( QObject* = 0 );
  SUIT_ProxyModel( SUIT_DataObject*, QObject* = 0 );
  SUIT_ProxyModel( SUIT_TreeModel*, QObject* = 0 );
  ~SUIT_ProxyModel();

  SUIT_DataObject*       root() const;
  void                   setRoot( SUIT_DataObject* );

  SUIT_DataObject*       object( const QModelIndex& = QModelIndex() ) const;
  QModelIndex            index( const SUIT_DataObject*, int = 0 ) const;

  bool                   autoDeleteTree() const;
  void                   setAutoDeleteTree( const bool );

  bool                   autoUpdate() const;
  void                   setAutoUpdate( const bool, const bool = true );
 
  bool                   isSortingEnabled() const;

  QAbstractItemDelegate* delegate() const;

public slots:
  virtual void           updateTree( const QModelIndex& );
  virtual void           updateTree( SUIT_DataObject* = 0 );
  void                   setSortingEnabled( bool );

protected:
  virtual bool           lessThan( const QModelIndex&, const QModelIndex& ) const;

private:
  SUIT_TreeModel*        treeModel() const;

private:
  bool                   mySortingEnabled;
};

class SUIT_EXPORT SUIT_ItemDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  SUIT_ItemDelegate( QObject* = 0 );
  
  virtual void paint( QPainter*, const QStyleOptionViewItem&,
		      const QModelIndex& ) const;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // SUIT_TREEMODEL_H
