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
// File   : OB_Browser.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "OB_Browser.h"

//#include "OB_Filter.h"
//#include "OB_ListItem.h"
//#include "OB_ListView.h"

#include <QtxTreeView.h>
//#include <SUIT_DataObjectIterator.h>

#include <QAction>
#include <QMenu>
#include <QItemSelection>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QAbstractItemDelegate>

#include <time.h>


/*!
  \class  OB_Browser::ToolTip
  Tool tip for OB_Browser.
*/

//TODO: ToolTip commented - to be removed or revised
/*
class OB_Browser::ToolTip : public QToolTip
{
public:
  ToolTip( OB_Browser* b, QWidget* p = 0 );
  virtual ~ToolTip();

  void        maybeTip( const QPoint& );

private:
  OB_Browser* myBrowser;
};
*/
/*!
  Constructor
*/
/*
OB_Browser::ToolTip::ToolTip( OB_Browser* b, QWidget* p )
: QToolTip( p ),
myBrowser( b )
{
}
*/
/*!
  Destructor
*/
/*
OB_Browser::ToolTip::~ToolTip()
{
}
*/
/*!
  It is called when there is a possibility that a tool tip
  should be shown and must decide whether there is a tool tip for the point
  in the widget that this QToolTip object relates to.
  \param pos - point co-ordinates
*/
/*
void OB_Browser::ToolTip::maybeTip( const QPoint& pos )
{
  if ( !parentWidget() || !myBrowser || !myBrowser->isShowToolTips() )
	  return;

  QListView* lv = myBrowser->listView();

  QListViewItem* item = lv->itemAt( pos );
  SUIT_DataObject* obj = myBrowser->dataObject( item );
  if ( !obj )
    return;

  QString aText = obj->toolTip();

  if ( aText.isEmpty() )
    return;

  QRect aRect = lv->itemRect( item );

  tip( aRect, aText );
}
*/


/*!
  \class OB_Browser
  Implements public API of an object browser that can be used to display arbitrary 
  application data in a hierarchical form. It is based on Qt4 model/view
  architecture. By default, object browser displays contents of SUIT_TreeModel, 
  which simply maps tree of data objects to a tree of model indices needed for QTreeView.
  However, an arbitrary model derived from QAbstractItemModel can be specified instead.
  For example, some SUIT-based application might not like using SUIT_DataObject class, 
  and in such a case it should provide its own implementation of a data model.
  
  Allows to get/set selection, 
  to drag-n-drop of objects, to determine item by SUIT object and 
  vice versa
*/

/*!
  \brief Constructor
  \param parent paren widget
  \param model data model
*/
OB_Browser::OB_Browser( QWidget* parent, QAbstractItemModel* model )
: QWidget( parent ),
  myAutoOpenLevel( 0 )
{
  myView = new QtxTreeView( this );
  myView->setRootIsDecorated( true );
  myView->setSelectionMode( QAbstractItemView::ExtendedSelection );
  myView->installEventFilter( this );
  myView->viewport()->installEventFilter( this );

  QVBoxLayout* main = new QVBoxLayout( this );
  main->addWidget( myView );
  main->setMargin( 0 );

  // TODO: decide what to do with tooltip
  //myShowToolTips = true;
  //myTooltip = new ToolTip( this, myView->viewport() );

  // TODO: drag-n-drop works differently - SUIT_TreeModel to be updated
  // and QTreeView needs some setup
  //connect( myView, SIGNAL( dropped( QPtrList<QListViewItem>, QListViewItem*, int ) ),
  //         this, SLOT( onDropped( QPtrList<QListViewItem>, QListViewItem*, int ) ) );
  connect( myView, SIGNAL( selectionChanged() ), this, SIGNAL( selectionChanged() ) );
  connect( myView, SIGNAL( doubleClicked( QListViewItem* ) ),
           this, SLOT( onDoubleClicked( QListViewItem* ) ) );

  setModel( model );
}

/*!
  \brief Destructor.
*/
OB_Browser::~OB_Browser()
{
  //delete myTooltip;
  //setUpdater( 0 );
}

/*!
  \return tree view of object browser
*/
QtxTreeView* OB_Browser::treeView() const
{
  return myView;
}

/*!
  \brief Get popup menu client type.
  \return popup client type
*/
QString OB_Browser::popupClientType() const
{
  return "ObjectBrowser";
}

/*!
  \brief Get data model.
  \return data model
  \sa setModel()
*/
QAbstractItemModel* OB_Browser::model() const
{
  return myView->model();
}

/*!
  \brief Set data model.
  \param model data model
  \sa model()
*/
void OB_Browser::setModel( QAbstractItemModel* model )
{
  myView->setModel( model );
  myView->update();
  setModified();
}

/*!
  \brief Get current item delegate.
  \return currently used item delegate
  \sa setItemDelegate()
*/
QAbstractItemDelegate* OB_Browser::itemDelegate() const
{
  return treeView() ? treeView()->itemDelegate() : 0;
}

/*!
  \brief Set item delegate.
  \param d custom item delegate
  \sa itemDelegate()
*/
void OB_Browser::setItemDelegate( QAbstractItemDelegate* d )
{
  if ( treeView() )
    treeView()->setItemDelegate( d );
}


/*!
  \brief Check if controls for expanding and collapsing top-level items are shown.
  \return \c true if top-level items are decorated
  \sa setRootIsDecorated()
*/
bool OB_Browser::rootIsDecorated() const
{
  return myView->rootIsDecorated();
}

/*!
  \brief Show/hide controls for expanding and collapsing top-level items.
  \param decor if \c true, top-level items are decorated
  \sa rootIsDecorated()
*/
void OB_Browser::setRootIsDecorated( const bool decor )
{
  if ( decor != rootIsDecorated() )
    myView->setRootIsDecorated( decor );
}

/*!
  \brief Get number of levels which should be automatically expanded
  when updating the data tree.
  \return number of levels to be auto-opened on tree updating
  \sa setAutoOpenLevel()
*/
int OB_Browser::autoOpenLevel() const
{
  return myAutoOpenLevel;
}

/*!
  \brief Set number of levels which should be automatically expanded
  when updating the data tree.
  \param levels number of levels to be auto-opened on tree updating
  \sa autoOpenLevel()
*/
void OB_Browser::setAutoOpenLevel( const int levels )
{
  if ( myAutoOpenLevel != levels )
    myAutoOpenLevel = levels;
}

/*!
  \brief Expand all branches to the specified number of levels.

  If \a levels value is negative, then autoOpenLevel() value is used instead.
  
  \param levels number of levels to be expanded
  \sa autoOpenLevel()
*/
void OB_Browser::openLevels( const int levels )
{
  treeView()->expandLevels( levels < 0 ? autoOpenLevel() : levels );
}

/*!
  \return state "are tooltips shown"
*/
/*
bool OB_Browser::isShowToolTips()
{
  return myShowToolTips;
}
*/
/*!
  Sets new value of state "are tooltips shown"
  \param theDisplay - new value
*/
/*
void OB_Browser::setShowToolTips( const bool theDisplay )
{
  myShowToolTips = theDisplay;
}
*/

/*!
  \return root SUIT object of browser
*/
/* TODO: removed
SUIT_DataObject* OB_Browser::getRootObject() const
{
  return myRoot;
}
*/
/*!
  Sets new root SUIT object of browser
  \param theRoot - new root object
*/
/* TODO: removed
void OB_Browser::setRootObject( SUIT_DataObject* theRoot )
{
  DataObjectKey curKey;
  DataObjectMap selObjs, openObjs;
  DataObjectKeyMap selKeys, openKeys;

  int selNum = numberOfSelected();

  SUIT_DataObject* curObj = 0;
  if ( theRoot )
    curObj = storeState( selObjs, openObjs, selKeys, openKeys, curKey );

  removeConnections( myRoot );
  if ( myRoot != theRoot && isAutoDeleteObjects() )
    delete myRoot;

  myRoot = theRoot;

  createConnections( myRoot );

  if ( myRoot )
    updateView( myRoot );
  else if ( listView() )
  {
    myItems.clear();
    listView()->clear();
  }

  restoreState( selObjs, openObjs, curObj, selKeys, openKeys, curKey );

  autoOpenBranches();

  setModified();

  if ( selNum != numberOfSelected() )
    emit selectionChanged();
}
*/
/*!
  \brief Get number of selected items.
  \return number of selected items
*/
int OB_Browser::numberOfSelected() const
{
  int count = 0;
  if ( treeView() && treeView()->selectionModel() )
     count = treeView()->selectionModel()->selectedIndexes().count();
  return count;
}

/*!
  \brief Get all selected items.
  \return unsorted list of selected indexes with no duplicates
*/
QModelIndexList OB_Browser::getSelected() const
{
  QModelIndexList selected = QModelIndexList();
  if ( treeView() && treeView()->selectionModel() )
    selected = treeView()->selectionModel()->selectedIndexes();
  return selected;
}

/*!
  \brief Get selection containing information about selected ranges.
  \return QItemSelection instance
*/
const QItemSelection OB_Browser::selection() const
{
  static QItemSelection emptySel;
  QItemSelection sel = emptySel;
  if ( treeView() && treeView()->selectionModel() )
    sel = treeView()->selectionModel()->selection();
  return sel;
}

/*!
  \brief Set selected objects.
  \param theObject - new selected object
  \param append - if it is true, then other selected objects are left as selected,
  otherwise only 'theObject' will be selected
*/
void OB_Browser::setSelected( const QModelIndex& theObject, const bool )
{
  /*
  DataObjectList lst;
  lst.append( theObject );
  setSelected( lst, append );
  */
}

/*!
  Sets selected objects
  \param theObjLst - new selected objects
  \param append - if it is true, then other selected objects are left as selected,
  otherwise only 'theObjLst' will be selected
*/
void OB_Browser::setSelected( const QModelIndexList& theObjLst, const bool )
{
  /*
  QListView* lv = listView();

  if ( !lv )
    return;

  bool changed = false;
  bool block = lv->signalsBlocked();
  lv->blockSignals( true );

  QMap<QListViewItem*, int> map;
  for ( DataObjectListIterator itr( theObjLst ); itr.current(); ++itr )
    map.insert( listViewItem( itr.current() ), 0 );

  for ( QListViewItemIterator it( lv ); it.current(); ++it )
  {
    QListViewItem* item = it.current();
    if ( map.contains( item ) && !lv->isSelected( item ) )
    {
      changed = true;
      lv->setSelected( item, true );
    }
    if ( !append && !map.contains( item ) && lv->isSelected( item ) )
    {
      changed = true;
      lv->setSelected( item, false );
    }
  }

  lv->blockSignals( block );

  if ( changed )
  {
    int count = 0;
    QListViewItem* sel = 0;
    QListViewItem* cur = lv->currentItem();
    for ( QListViewItemIterator iter( lv ); iter.current() && !sel; ++iter, count++ )
    {
      if ( iter.current()->isSelected() && cur == iter.current() )
        sel = iter.current();
    }

    for ( QListViewItemIterator itr( lv ); itr.current() && !sel; ++itr )
    {
      if ( itr.current()->isSelected() )
	      sel = itr.current();
    }

    if ( sel )
      lv->setCurrentItem( sel );

    if ( sel && count == 1 )
      lv->ensureItemVisible( sel );

    emit selectionChanged();
  }
  */
}

/*!
  \return true if item corresponding to object is opened
  \param index - object to be checked
*/
bool OB_Browser::isOpen( const QModelIndex& index ) const
{
  return treeView()->isExpanded( index );
}

/*!
  Sets opened state of item
  \param index - object corresponding to item
  \param open - new opened state
*/
void OB_Browser::setOpen( const QModelIndex& index, const bool open )
{
  treeView()->setExpanded( index, open );
}

/*!
  Adjusts width by root item
*/
void OB_Browser::adjustWidth()
{
  treeView()->resizeColumnToContents( 0 );
}

/*!
  \return SUIT object correspondint to item at position 'pos'
  \param pos - position
*/
/* TODO: removed - QTreeView::indexAt() should be used
SUIT_DataObject* OB_Browser::dataObjectAt( const QPoint& pos ) const
{
  SUIT_DataObject* obj = 0;

  QListView* lv = listView();
  if ( lv )
    obj = dataObject( lv->itemAt( pos ) );

  return obj;
}
*/
/*!
  \return filter of list view
*/
/* TODO: removed
OB_Filter* OB_Browser::filter() const
{
  return myView->filter();
}
*/
/*!
  Changes filter of list view
  \param f - new filter
*/
/* TODO: removed
void OB_Browser::setFilter( OB_Filter* f )
{
  myView->setFilter( f );
}
*/
/*!
  Adds new column to list view
  \param label - title of column
  \param id - id of column
  \param width - width of column
*/
/* TODO: removed
int OB_Browser::addColumn( const QString& label, const int id, const int width )
{
  return addColumn( QIconSet(), label, id, width );
}
*/
/*!
  Adds new column to list view
  \param icon - icon of column
  \param label - title of column
  \param id - id of column
  \param width - width of column
*/
/* TODO: removed
int OB_Browser::addColumn( const QIconSet& icon, const QString& label, const int id, const int width )
{
  QListView* lv = listView();
  if ( !lv )
    return -1;

  int theId = id;
  if ( theId < 0 )
  {
    while ( myColumnIds.contains( theId ) )
      theId++;
  }

  if ( myColumnIds.contains( theId ) )
    return -1; // can not reuse id

  int sec = -1;
  if ( icon.isNull() )
    sec = lv->addColumn( label, width );
  else
    sec = lv->addColumn( icon, label, width );

  if ( sec == -1 )
    return -1;

  myColumnIds.insert( theId, sec );
  updateText();

  return theId;
}
*/
/*!
  Removes column
  \param id - id of column
*/
/* TODO: removed
void OB_Browser::removeColumn( const int id )
{
  QListView* lv = listView();
  if ( !lv || !myColumnIds.contains( id ) )
    return;

  int sec = myColumnIds[id];
  lv->removeColumn( sec );

  // update map of column indeces
  myColumnIds.remove( id );
  for ( QMap<int, int>::iterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
  {
    if ( it.key() > id )
      it.data()--;
  }
  updateText();
}
*/
/*!
  Sets title of first column (name column)
  \param label - new title
*/
/* TODO: removed
void OB_Browser::setNameTitle( const QString& label )
{
  setNameTitle( QIconSet(), label );
}
*/
/*!
  Sets title and icon of first column (name column)
  \param icon - new icon
  \param label - new title
*/
/* TODO: removed
void OB_Browser::setNameTitle( const QIconSet& icon, const QString& label )
{
  QListView* lv = listView();
  if ( !lv )
    return;

  if ( icon.isNull() )
    lv->setColumnText( 0, label );
  else
    lv->setColumnText( 0, icon, label );
}
*/
/*!
  Sets title of column
  \param id - column id
  \param label - new column title
*/
/* TODO: removed
void OB_Browser::setColumnTitle( const int id, const QString& label )
{
  setColumnTitle( id, QIconSet(), label );
}
*/
/*!
  Sets title and icon of column
  \param id - column id
  \param icon - new column icon
  \param label - new column title
*/
/* TODO: removed
void OB_Browser::setColumnTitle( const int id, const QIconSet& icon, const QString& label )
{
  QListView* lv = listView();
  if ( !lv || !myColumnIds.contains( id ) )
    return;

  if ( icon.isNull() )
    lv->setColumnText( myColumnIds[id], label );
  else
    lv->setColumnText( myColumnIds[id], icon, label );
}
*/
/*!
  \return title of first column (name column)
*/
/* TODO: removed
QString OB_Browser::nameTitle() const
{
  return myView->columnText( 0 );
}
*/
/*!
  \return title of first column (name column)
  \param id - column id
*/
/* TODO: removed
QString OB_Browser::columnTitle( const int id ) const
{
  QString txt;
  if ( myColumnIds.contains( id ) )
    txt = myView->columnText( myColumnIds[id] );
  return txt;
}
*/
/*!
  \return true if column is visible
  \param id - column id
*/
/* TODO: removed
bool OB_Browser::isColumnVisible( const int id ) const
{
  return myColumnIds.contains( id ) && myView->isShown( myColumnIds[id] );
}
*/
/*!
  Sets visibility of column
  \param id - column id
  \param on - new visibility state
*/
/* TODO: removed
void OB_Browser::setColumnShown( const int id, const bool on )
{
  if ( !myColumnIds.contains( id ) )
    return;

  myView->setShown( myColumnIds[id], on );
  if( !on )
    myView->setColumnWidthMode( myColumnIds[id], QListView::Manual );
}
*/
/*!
  Sets global width mode
  \param mode - new width mode
*/
/* TODO: removed
void OB_Browser::setWidthMode( QListView::WidthMode mode )
{
  for ( int i = 0, n = myView->columns(); i < n; i++ )
    if( mode!=QListView::Maximum || myView->columnWidth( i )>0 )
      myView->setColumnWidthMode( i, mode );
}
*/
/*!
  \return list of columns ids
*/
/* TODO: to be revised or removed
QValueList<int> OB_Browser::columns() const
{
  QValueList<int> lst;
  for ( QMap<int, int>::ConstIterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
    lst.append( it.key() );
  return lst;
}
*/
/*!
  \return true if it is possible to show/hide column by popup
  \param id - column id
*/
/* TODO: removed
bool OB_Browser::appropriateColumn( const int id ) const
{
  bool res = false;
  if ( myColumnIds.contains( id ) )
    res = myView->appropriate( myColumnIds[id] );
  return res;
}
*/
/*!
  Sets "appropriate state": is it possible to show/hide column by popup
  \param id - column id
  \param on - new state
*/
/* TODO: removed
void OB_Browser::setAppropriateColumn( const int id, const bool on )
{
  if ( !myColumnIds.contains( id ) )
    return;

  myView->setAppropriate( myColumnIds[id], on );
}
*/
/*!
  Updates tree
  \param obj - start object
  \param autoOpen - to open automatically branches of autoOpenLevel()
  \sa autoOpenLevel()
*/
/* TODO: removed
void OB_Browser::updateTree( SUIT_DataObject* obj, const bool autoOpen )
{
//  QTime t1 = QTime::currentTime();

  if ( !obj && !(obj = getRootObject()) )
    return;

  DataObjectKey curKey;
  DataObjectMap selObjs, openObjs;
  DataObjectKeyMap selKeys, openKeys;

  int selNum = numberOfSelected();

  SUIT_DataObject* curObj = storeState( selObjs, openObjs, selKeys, openKeys, curKey );

  updateView( obj );

  restoreState( selObjs, openObjs, curObj, selKeys, openKeys, curKey );

  if ( autoOpen )
    autoOpenBranches();

  setModified();

  if ( selNum != numberOfSelected() )
    emit selectionChanged();

//  QTime t2 = QTime::currentTime();
//  qDebug( QString( "update tree time = %1 msecs" ).arg( t1.msecsTo( t2 ) ) );
}
*/
/*!
  Replaces part of tree starting at object 'src' by tree starting at object 'trg'
*/
/* TODO: removed
void OB_Browser::replaceTree( SUIT_DataObject* src, SUIT_DataObject* trg )
{
  if ( !src || !trg || src == trg || src->root() != getRootObject() )
    return;

  DataObjectKey curKey;
  DataObjectMap selObjs, openObjs;
  DataObjectKeyMap selKeys, openKeys;

  int selNum = numberOfSelected();

  SUIT_DataObject* curObj = storeState( selObjs, openObjs, selKeys, openKeys, curKey );

  SUIT_DataObject* parent = src->parent();
  int pos = parent ? parent->childPos( src ) : -1;

  src->setParent( 0 );

  removeConnections( src );
  if ( isAutoDeleteObjects() )
    delete src;

  if ( parent && pos != -1 )
    parent->insertChild( trg, pos );

  trg->setParent( parent );

  updateView( trg );
  createConnections( trg );

  restoreState( selObjs, openObjs, curObj, selKeys, openKeys, curKey );

  setModified();

  if ( selNum != numberOfSelected() )
    emit selectionChanged();
}
*/
/*!
  Updates view
  \param startObj - start object
*/
/* TODO: removed
void OB_Browser::updateView( SUIT_DataObject* startObj )
{
  QListView* lv = listView();
  if ( !lv )
    return;

  if ( !startObj || startObj->root() != getRootObject() )
    return;

  //qDebug( "updateView:" );
  //startObj->dump();

  if ( startObj == myRoot )
  {
    OB_BrowserSync sync( this );
    synchronize<ObjPtr,ItemPtr,OB_BrowserSync>( myRoot, 0, sync );
  }
  else
  {
    OB_BrowserSync sync( this );
    OB_ListItem* startItem = dynamic_cast<OB_ListItem*>( listViewItem( startObj ) );
    synchronize<ObjPtr,ItemPtr,OB_BrowserSync>( startObj, startItem, sync );
  }
}
*/

/*!
  Adjusts width by item
  \param item
*/
/*
void OB_Browser::adjustWidth( QListViewItem* item )
{
  while ( item )
  {
    item->widthChanged( 0 );
    if ( item->isOpen() )
      adjustWidth( item->firstChild() );
    item = item->nextSibling();
  }
}
*/

/*!
  \remove all items referencing current (through data objects)
*/
/* TODO:
void OB_Browser::removeReferences( QListViewItem* item )
{
  if ( !item )
    return;

  SUIT_DataObject* obj = dataObject( item );
  obj->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
  myItems.remove( obj );

  QListViewItem* i = item->firstChild();
  while ( i )
  {
    removeReferences( i );
    i = i->nextSibling();
  }
}
*/
/*!
  Connects all children to SLOT onDestroyed
*/
/* TODO: move to SUIT_TreeModel
void OB_Browser::createConnections( SUIT_DataObject* obj )
{
  if ( !obj )
    return;

  DataObjectList childList;
  obj->children( childList, true );

  childList.prepend( obj );

  for ( DataObjectListIterator it( childList ); it.current(); ++it )
    it.current()->connect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
}
*/
/*!
  Disconnects all children from SLOT onDestroyed
*/
/* TODO: move to SUIT_TreeModel
void OB_Browser::removeConnections( SUIT_DataObject* obj )
{
  if ( !obj )
    return;

  DataObjectList childList;
  obj->children( childList, true );

  childList.prepend( obj );

  for ( DataObjectListIterator it( childList ); it.current(); ++it )
    it.current()->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
}
*/
/*!
  Stores states (opened, selected) of current tree items
  \return current item
  \param selObjs, selKeys - maps of selected objects
  \param openObjs, openKeys - maps of opened objects
  \param curKey - map of current objects
*/
/* TODO: to be revised
SUIT_DataObject* OB_Browser::storeState( DataObjectMap& selObjs, DataObjectMap& openObjs,
                                         DataObjectKeyMap& selKeys, DataObjectKeyMap& openKeys,
                                         DataObjectKey& curKey ) const
{
  QListView* lv = listView();
  if ( !lv )
    return 0;

  SUIT_DataObject* curObj = dataObject( lv->currentItem() );

  curKey = objectKey( curObj );

  for ( QListViewItemIterator it( lv ); it.current(); ++it )
  {
    SUIT_DataObject* obj = dataObject( it.current() );
    if ( !obj )
      continue;

    selObjs.insert( obj, lv->isSelected( it.current() ) );
    openObjs.insert( obj, lv->isOpen( it.current() ) );
    if ( lv->isSelected( it.current() ) )
      selKeys.insert( objectKey( obj ), 0 );
    if ( lv->isOpen( it.current() ) )
      openKeys.insert( objectKey( obj ), 0 );
  }

  return curObj;
}
*/
/*!
  Restores states (opened, selected) of current tree items
  \param selObjs, selKeys - maps of selected objects
  \param openObjs, openKeys - maps of opened objects
  \param curKey - map of current objects
*/
/* TODO: to be revised
void OB_Browser::restoreState( const DataObjectMap& selObjs, const DataObjectMap& openObjs,
                               const SUIT_DataObject* curObj, const DataObjectKeyMap& selKeys,
                               const DataObjectKeyMap& openKeys, const DataObjectKey& curKey )
{
  QListView* lv = listView();
  if ( !lv )
    return;

  bool block = lv->signalsBlocked();
  lv->blockSignals( true );

  QListViewItem* curItem = 0;
  for ( QListViewItemIterator it( lv ); it.current(); ++it )
  {
    QListViewItem* item = it.current();
    SUIT_DataObject* obj = dataObject( item );

    if ( !obj )
      continue;

    DataObjectKey key = objectKey( obj );

    if ( selObjs.contains( obj ) )
    {
      if ( selObjs[obj] && !lv->isSelected( item ) )
        lv->setSelected( item, true );
    }
    else if ( !key.isNull() && selKeys.contains( key ) && !lv->isSelected( item ) )
      lv->setSelected( item, true );

    if ( openObjs.contains( obj ) )
    {
      bool parentOpen = true;
      if( item && item->parent() )
	parentOpen = item->parent()->isOpen();

      if ( openObjs[obj] && parentOpen )
        lv->setOpen( item, true );
    }
    else if ( !key.isNull() && openKeys.contains( key ) )
    {
      bool parentOpen = true;
      if( item && item->parent() )
	parentOpen = item->parent()->isOpen();

      if( parentOpen )
	lv->setOpen( item, true );
    }

    if ( !curItem && ( curObj == obj || ( !curKey.isNull() && curKey == key )) )
      curItem = item;
  }

  if ( curItem )
    lv->setCurrentItem( curItem );

  lv->blockSignals( block );
}
*/
/*!
  Creates object key by tree item
*/
/* TODO: move to SUIT_TreeModel
OB_Browser::DataObjectKey OB_Browser::objectKey( QListViewItem* i ) const
{
  return objectKey( dataObject( i ) );
}
*/
/*!
  Creates object key by SUIT object
*/
/* TODO: move to SUIT_TreeModel
OB_Browser::DataObjectKey OB_Browser::objectKey( SUIT_DataObject* obj ) const
{
  if ( !obj )
    return 0;

  return DataObjectKey( obj->key() );
}
*/

/*!
  \brief Process context menu request event.
  \param e context menu event
*/
void OB_Browser::contextMenuEvent( QContextMenuEvent* e )
{
  contextMenuRequest( e );
}

/*!
  \brief Called when "Expand all" popup menu command is activated.
  
  Expands all selected items recursively.
*/
void OB_Browser::onExpand()
{
  QModelIndexList indexes = treeView()->selectionModel()->selectedIndexes();
  QModelIndex index;

  foreach ( index, indexes ) {
    treeView()->expandAll( index );
  }
}

/*!
  SLOT: called if action "Show/hide column" is activated by popup
*/
/* TODO: removed
void OB_Browser::onColumnVisible( int id )
{
  setColumnShown( id, !isColumnVisible( id ) );
}
*/
/*!
  SLOT: called if SUIT object is destroyed
*/
/* TODO: moved to SUIT_TreeModel
void OB_Browser::onDestroyed( SUIT_DataObject* obj )
{
  removeObject( obj );
}
*/
/*!
  SLOT: called on finish of drag-n-drop operation
  \param items - dragged items
  \param item - destination (item on that they were dropped)
  \param action - QDropEvent::Action
*/
  // TODO: drag-n-drop works differently - SUIT_TreeModel to be updated
  // and QTreeView needs some setup
/*
void OB_Browser::onDropped( QPtrList<QListViewItem> items, QListViewItem* item, int action )
{
  SUIT_DataObject* obj = dataObject( item );
  if ( !obj )
    return;

  DataObjectList lst;
  for ( QPtrListIterator<QListViewItem> it( items ); it.current(); ++it )
  {
    SUIT_DataObject* o = dataObject( it.current() );
    if ( o )
      lst.append( o );
  }

  if ( !lst.isEmpty() )
    emit dropped( lst, obj, action );
}
*/
/*!
  Updates texts of items
*/
/* TODO: to be removed
void OB_Browser::updateText()
{
  if ( myColumnIds.isEmpty() )
    return;

  QListView* lv = listView();
  if ( !lv )
    return;

  for ( QListViewItemIterator it( lv ); it.current(); ++it )
  {
    SUIT_DataObject* obj = dataObject( it.current() );
    if ( !obj )
      continue;

    for( QMap<int, int>::iterator itr = myColumnIds.begin(); itr != myColumnIds.end(); ++itr )
      it.current()->setText( itr.data(), obj->text( itr.key() ) );
  }
}
*/
/*!
  \return true if item must be updated
  \param item - item to be checked
*/
/* TODO: to be revised
bool OB_Browser::needToUpdateTexts( QListViewItem* item ) const
{
  SUIT_DataObject* obj = dataObject( item );
  if ( !obj )
    return false;

  for( QMap<int, int>::const_iterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
    if( item->text( it.data() ) != obj->text( it.key() ) )
      return true;
  return false;
}
*/
/*!
  Updates texts of item
  \param item - item to be updated
*/
/* TODO: to be revised
void OB_Browser::updateText( QListViewItem* item )
{
  SUIT_DataObject* obj = dataObject( item );
  if ( !obj )
    return;

  for( QMap<int, int>::iterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
    item->setText( it.data(), obj->text( it.key() ) );
}
*/
/*!
  Adds custom actions to popup
  \param menu - popup menu
*/
void OB_Browser::contextMenuPopup( QMenu* menu )
{
  menu->addSeparator();

  QModelIndexList indexes = treeView()->selectionModel()->selectedIndexes();

  bool closed = false;
  
  for ( QModelIndexList::Iterator it = indexes.begin(); 
	it != indexes.end() && !closed; ++it ) {
    closed = hasCollased( *it );
  }

  if ( closed )
    menu->addAction( tr( "MEN_EXPAND_ALL" ), this, SLOT( onExpand() ) );

  menu->addSeparator();
}

/*!
  Expands item with all it's children
*/
/* TODO: to be revised
void OB_Browser::expand( QListViewItem* item )
{
  if ( !item )
    return;

  item->setOpen( true );
  for ( QListViewItem* child = item->firstChild(); child; child = child->nextSibling() )
    expand( child );
}
*/
/*!
  \return true if item or one of it's children isn't opened
*/
bool OB_Browser::hasCollased( const QModelIndex& index ) const
{
  bool result = false;

  if ( index.isValid() ) {
    bool hasChildren = treeView()->model()->hasChildren( index );
    result = hasChildren && !treeView()->isExpanded( index );
    if ( !result && hasChildren ) {
      int rows = treeView()->model()->rowCount( index );
      for ( int i = 0; i < rows && !result; i ++ ) {
	QModelIndex child = treeView()->model()->index( i, 0, index );
	result = hasCollased( child );
      }
    }
  }
  return result;
}
/*!
  Removes SUIT object
  \param obj - SUIT object to be removed
  \param autoUpd - auto tree updating
*/
/* TODO: moved to SUIT_TreeModel
void OB_Browser::removeObject( SUIT_DataObject* obj, const bool autoUpd )
{
  if ( !obj )
    return;

  // Removing list view items from <myItems> recursively for all children.
  // Otherwise, "delete item" line will destroy all item's children,
  // and <myItems> will contain invalid pointers (see ~QListViewItem() description in Qt docs)
  DataObjectList childList;
  obj->children( childList, true );
  for ( DataObjectListIterator it( childList ); it.current(); ++it )
  {
    it.current()->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
    myItems.remove( it.current() );
  }

  QListViewItem* item = listViewItem( obj );

  obj->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
  myItems.remove( obj );

  if ( obj == myRoot )
  {
    // remove all child list view items
    setRootObject( 0 );
    return;
  }

  if( !autoUpd )
    return;

  if ( isAutoUpdate() )
  {
    SUIT_DataObject* pObj = item && item->parent() ? dataObject( item->parent() ) : 0;
    updateTree( pObj, false );
  }

  delete item;
}
*/
/*!
  Opens branches from 1 to autoOpenLevel()
  \sa autoOpenLevel()
*/
/* TODO: to be revised
void OB_Browser::autoOpenBranches()
{
  openLevels();
}
*/
/*!
  Opens branch
  \param item
  \param level
*/
/* TODO: to be revised
void OB_Browser::openBranch( QListViewItem* item, const int level )
{
  if ( level < 1 )
    return;

  while ( item )
  {
    item->setOpen( true );
    openBranch( item->firstChild(), level - 1 );
    item = item->nextSibling();
  }
}
*/
/*!
  SLOT: called on double click on item, emits signal
*/
/* TODO: to be revised
void OB_Browser::onDoubleClicked( QListViewItem* item )
{
  if ( item )
    emit doubleClicked( dataObject( item ) );
}
*/
/*!
  Stores time of last modification
*/
void OB_Browser::setModified()
{
  myModifiedTime = clock();
}
