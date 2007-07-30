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
// File   : OB_Browser.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef OB_BROWSER_H
#define OB_BROWSER_H

#include "OB.h"

#include <QWidget>
#include <QMap>
#include <QModelIndex>
#include <QItemSelection>

#include <SUIT_DataObject.h>
#include <SUIT_DataObjectKey.h>
#include <SUIT_PopupClient.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QAbstractItemModel;
class QToolTip;
class QtxTreeView;

class OB_EXPORT OB_Browser : public QWidget, public SUIT_PopupClient
{
  Q_OBJECT

  // TODO: commented - to be removed or revised
  //class ToolTip;

public:
  OB_Browser( QWidget* = 0, QAbstractItemModel* = 0 );
  virtual ~OB_Browser();

  QtxTreeView*         treeView() const;

  virtual QString      popupClientType() const;

  QAbstractItemModel*  model() const;
  void                 setModel( QAbstractItemModel* );

  bool                 rootIsDecorated() const;
  void                 setRootIsDecorated( const bool );

  int                  autoOpenLevel() const;
  void                 setAutoOpenLevel( const int );
  void                 openLevels( const int = -1 );

  //bool                 isShowToolTips();
  //void                 setShowToolTips( const bool theDisplay );

  int                  numberOfSelected() const;
  QModelIndexList      getSelected() const;
  const QItemSelection selection() const;

  virtual void         setSelected( const QModelIndex&, const bool = false );
  virtual void         setSelected( const QModelIndexList&, const bool = false );

  bool                 isOpen( const QModelIndex& ) const;
  virtual void         setOpen( const QModelIndex& theObject, const bool theOpen = true );

  void                 adjustWidth();

  // san - To be revised or removed
  // QTreeView::indexAt() should be used
  //SUIT_DataObject*  dataObjectAt( const QPoint& ) const;

  // san - Removed
  //OB_Filter*          filter() const;
  //void                setFilter( OB_Filter* );

  // san - Removed, columns and titles are controlled by SUIT_TreeModel and SUIT_DataObject
  //virtual int       addColumn( const QString&, const int id = -1, const int width = -1 );
  //virtual int       addColumn( const QIconSet&, const QString&, const int id = -1, const int width = -1 );
  //virtual void      removeColumn( const int id );

  //void              setNameTitle( const QString& );
  //virtual void      setNameTitle( const QIconSet&, const QString& );
  //void              setColumnTitle( const int id, const QString& );
  //virtual void      setColumnTitle( const int id, const QIconSet&, const QString& );

  //QString           nameTitle() const;
  //QString           columnTitle( const int ) const;

  // san - These methods are now in QTreeView
  //bool              isColumnVisible( const int ) const;
  //virtual void      setColumnShown( const int, const bool );

  // TODO: QTreeView::resizeColumnToContents() can be used instead
  //virtual void      setWidthMode( QListView::WidthMode );

  // san - removed
  // QValueList<int>   columns() const;

  //bool              appropriateColumn( const int ) const;
  //virtual void      setAppropriateColumn( const int, const bool );

  //virtual bool      eventFilter(QObject* watched, QEvent* e);

  virtual void      contextMenuPopup( QMenu* );

  void              setModified();
  unsigned long     getModifiedTime() { return myModifiedTime; }
  
  // san - moved to SUIT_TreeModel
  //OB_Updater*       getUpdater() const;
  //virtual void      setUpdater( OB_Updater* theUpdate = 0 );

signals:
  void              selectionChanged();
  void              doubleClicked( SUIT_DataObject* );
  void              dropped( DataObjectList, SUIT_DataObject*, int );

private slots:
  //void              onExpand();
  //void              onColumnVisible( int );
  //void              onDestroyed( SUIT_DataObject* );
  //void              onDoubleClicked ( QListViewItem* );
  //void              onDropped( QPtrList<QListViewItem>, QListViewItem*, int );

protected:
  //void              adjustWidth( QListViewItem* );
  //virtual void      updateView( SUIT_DataObject* = 0 );
  //virtual void      updateText();

  virtual void      keyPressEvent( QKeyEvent* );

private:
  //typedef QMap<SUIT_DataObject*, QListViewItem*> ItemMap;
  //typedef SUIT_DataObjectKeyHandle               DataObjectKey;
  //typedef QMap<SUIT_DataObject*, int>            DataObjectMap;
  //typedef QMap<DataObjectKey, int>               DataObjectKeyMap;

private:
  //void              expand( QListViewItem* );
  //bool              hasClosed( QListViewItem* ) const;

  //void              autoOpenBranches();
  //void              openBranch( QListViewItem*, const int );

  //void              removeReferences( QListViewItem* );
  //void              removeConnections( SUIT_DataObject* );
  //void              createConnections( SUIT_DataObject* );
  //void              removeObject( SUIT_DataObject*, const bool = true );

  //void              updateText( QListViewItem* );
  //bool              needToUpdateTexts( QListViewItem* ) const;

  //DataObjectKey     objectKey( QListViewItem* ) const;
  //DataObjectKey     objectKey( SUIT_DataObject* ) const;

  //QListViewItem*    createTree( const SUIT_DataObject*, QListViewItem*, QListViewItem* = 0, const bool = false );
  //QListViewItem*    createItem( const SUIT_DataObject*, QListViewItem*, QListViewItem* = 0, const bool = false );

  //SUIT_DataObject*  storeState( DataObjectMap&, DataObjectMap&,
  //                              DataObjectKeyMap&, DataObjectKeyMap&, DataObjectKey& ) const;
  //void              restoreState( const DataObjectMap&, const DataObjectMap&, const SUIT_DataObject*,
  //                                const DataObjectKeyMap&, const DataObjectKeyMap&, const DataObjectKey& );

private:
  QtxTreeView*        myView;
  // TODO: decide what to do with tooltip
  //QToolTip*           myTooltip;
  //QMap<int, int>      myColumnIds;
  // TODO: decide what to do with tooltip
  //bool                myShowToolTips;
  int                 myAutoOpenLevel;
  unsigned long       myModifiedTime;

  // TODO: decide what to do with tooltip
  //friend class OB_Browser::ToolTip;
};

#endif  // OB_BROWSER_H
