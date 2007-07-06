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
// File:      QtxPagePrefMgr.cxx
// Author:    Sergey TELKOV

#include "QtxPagePrefMgr.h"

#include "QtxGridBox.h"
#include "QtxFontEdit.h"
#include "QtxGroupBox.h"
#include "QtxComboBox.h"
#include "QtxIntSpinBox.h"
#include "QtxColorButton.h"
#include "QtxDoubleSpinBox.h"

#include <QEvent>
#include <QLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QSplitter>
#include <QTabWidget>
#include <QListWidget>
#include <QStackedWidget>

/*!
  \class QtxPagePrefMgr
  GUI implementation of QtxPreferenceMgr - manager of preferences
*/

QtxPagePrefMgr::QtxPagePrefMgr( QtxResourceMgr* resMgr, QWidget* parent )
: QFrame( parent ),
QtxPreferenceMgr( resMgr ),
myInit( false )
{
  myBox = new QtxGridBox( 1, Qt::Horizontal, this, 0 );
  QVBoxLayout* base = new QVBoxLayout( this );
  base->setMargin( 0 );
  base->setSpacing( 0 );
  base->addWidget( myBox );
}

QtxPagePrefMgr::~QtxPagePrefMgr()
{
}

QSize QtxPagePrefMgr::sizeHint() const
{
  initialize();

  return QFrame::sizeHint();
}

QSize QtxPagePrefMgr::minimumSizeHint() const
{
  initialize();

  return QFrame::minimumSizeHint();
}

void QtxPagePrefMgr::setVisible( bool on )
{
  if ( on && !myInit )
    updateContents();

  QFrame::setVisible( on );
}

void QtxPagePrefMgr::updateContents()
{
  QtxPreferenceMgr::updateContents();

  QList<QtxPreferenceItem*> lst = childItems();
  for ( QList<QtxPreferenceItem*>::const_iterator it = lst.begin(); it != lst.end(); ++it )
  {
    if ( (*it)->rtti() == QtxPagePrefItem::RTTI() )
    {
      QtxPagePrefItem* item = (QtxPagePrefItem*)(*it);
      if ( item->widget() && item->widget()->parent() != myBox )
        item->widget()->setParent( myBox );
    }
  }
}

void QtxPagePrefMgr::itemAdded( QtxPreferenceItem* )
{
  triggerUpdate();
}

void QtxPagePrefMgr::itemRemoved( QtxPreferenceItem* )
{
  triggerUpdate();
}

void QtxPagePrefMgr::itemChanged( QtxPreferenceItem* )
{
  triggerUpdate();
}

QVariant QtxPagePrefMgr::optionValue( const QString& name ) const
{
  if ( name == "orientation" )
    return myBox->orientation() == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal;
  else
    return QtxPreferenceMgr::optionValue( name );
}

void QtxPagePrefMgr::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "orientation" )
  {
    if ( val.canConvert( QVariant::Int ) )
      myBox->setOrientation( val.toInt() == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal );
  }
  else
    QtxPreferenceMgr::setOptionValue( name, val );
}

void QtxPagePrefMgr::initialize() const
{
  if ( myInit )
    return;

  QtxPagePrefMgr* that = (QtxPagePrefMgr*)this;
    
  that->updateContents();

  QList<QtxPreferenceItem*> lst = childItems( true );
  for ( QList<QtxPreferenceItem*>::iterator it = lst.begin(); it != lst.end(); ++it )
    (*it)->updateContents();

  that->myInit = true;
}

/*!
  \class QtxPagePrefItem
  Base class for implementation of the preference items
*/

QtxPagePrefItem::QtxPagePrefItem( const QString& title, QtxPreferenceItem* parent,
                                  const QString& sect, const QString& param )
: QtxPreferenceItem( title, sect, param, parent ),
myWidget( 0 )
{
}

QtxPagePrefItem::~QtxPagePrefItem()
{
  delete myWidget;
}

int QtxPagePrefItem::rtti() const
{
  return QtxPagePrefItem::RTTI();
}

QWidget* QtxPagePrefItem::widget() const
{
  return myWidget;
}

int QtxPagePrefItem::RTTI()
{
  return 1000;
}

void QtxPagePrefItem::setWidget( QWidget* wid )
{
  myWidget = wid;
  sendItemChanges();
}

void QtxPagePrefItem::itemAdded( QtxPreferenceItem* )
{
  contentChanged();
}

void QtxPagePrefItem::itemRemoved( QtxPreferenceItem* )
{
  contentChanged();
}

void QtxPagePrefItem::itemChanged( QtxPreferenceItem* )
{
  contentChanged();
}

void QtxPagePrefItem::store()
{
}

void QtxPagePrefItem::retrieve()
{
}

void QtxPagePrefItem::pageChildItems( QList<QtxPagePrefItem*>& list, const bool rec ) const
{
  QList<QtxPreferenceItem*> lst = childItems( rec );
  for ( QList<QtxPreferenceItem*>::const_iterator it = lst.begin(); it != lst.end(); ++it )
  {
    if ( (*it)->rtti() == QtxPagePrefItem::RTTI() )
      list.append( (QtxPagePrefItem*)*it );
  }
}

void QtxPagePrefItem::contentChanged()
{
  triggerUpdate();
}

/*!
  \class QtxPageNamedPrefItem
  Base class for implementation of the named preference items (items with text labels).
*/
QtxPageNamedPrefItem::QtxPageNamedPrefItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param ),
myControl( 0 )
{
  QWidget* main = new QWidget();
  QHBoxLayout* base = new QHBoxLayout( main );
  base->setMargin( 0 );
  base->setSpacing( 5 );

  myLabel = new QLabel( title, main );
  base->addWidget( myLabel );

  setWidget( main );

  myLabel->setVisible( !title.isEmpty() );
}

QtxPageNamedPrefItem::~QtxPageNamedPrefItem()
{
}

void QtxPageNamedPrefItem::setTitle( const QString& txt )
{
  QtxPagePrefItem::setTitle( txt );

  label()->setText( title() );
  if ( !title().isEmpty() )
    label()->setVisible( true );
}

QLabel* QtxPageNamedPrefItem::label() const
{
  return myLabel;
}

QWidget* QtxPageNamedPrefItem::control() const
{
  return myControl;
}

void QtxPageNamedPrefItem::setControl( QWidget* wid )
{
  if ( myControl == wid )
    return;

  delete myControl;
  myControl = wid;

  if ( myControl )
    widget()->layout()->addWidget( myControl );
}

/*!
  \class QtxPagePrefListItem
*/

QtxPagePrefListItem::QtxPagePrefListItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param ),
myFix( false )
{
  QSplitter* main = new QSplitter( Qt::Horizontal );
  main->setChildrenCollapsible( false );

  main->addWidget( myList = new QListWidget( main ) );
  main->addWidget( myStack = new QStackedWidget( main ) );

  myList->setSelectionMode( QListWidget::SingleSelection );

  myStack->addWidget( myInfLabel = new QLabel( myStack ) );
  myInfLabel->setAlignment( Qt::AlignCenter );

  connect( myList, SIGNAL( itemSelectionChanged() ), this, SLOT( onItemSelectionChanged() ) );

  setWidget( main );
}

QtxPagePrefListItem::~QtxPagePrefListItem()
{
}

QString QtxPagePrefListItem::emptyInfo() const
{
  return myInfText;
}

void QtxPagePrefListItem::setEmptyInfo( const QString& inf )
{
  if ( myInfText == inf )
    return;

  myInfText = inf;

  updateVisible();
}

bool QtxPagePrefListItem::isFixedSize() const
{
  return myFix;
}

void QtxPagePrefListItem::setFixedSize( const bool on )
{
  if ( myFix == on )
    return;

  myFix = on;

  updateGeom();
}

void QtxPagePrefListItem::updateContents()
{
  updateVisible();
}

QVariant QtxPagePrefListItem::optionValue( const QString& name ) const
{
  if ( name == "fixed_size" )
    return isFixedSize();
  else if ( name == "empty_info" || name == "info" )
    return emptyInfo();
  else
    return QtxPagePrefItem::optionValue( name );
}

void QtxPagePrefListItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "fixed_size" )
  {
    if ( val.canConvert( QVariant::Bool ) )
      setFixedSize( val.toBool() );
  }
  else if ( name == "empty_info" || name == "info" )
  {
    if ( val.canConvert( QVariant::String ) )
      setEmptyInfo( val.toString() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

void QtxPagePrefListItem::onItemSelectionChanged()
{
  updateState();
}

void QtxPagePrefListItem::updateInfo()
{
  QString infoText;
  QtxPagePrefItem* item = selectedItem();
  if ( item )
  {
    infoText = emptyInfo();
    QRegExp rx( "%([%|N])" );

    int idx = 0;
    while ( ( idx = rx.indexIn( infoText ) ) != -1 )
    {
      if ( rx.cap() == QString( "%%" ) )
        infoText.replace( idx, rx.matchedLength(), "%" );
      else if ( rx.cap() == QString( "%N" ) )
        infoText.replace( idx, rx.matchedLength(), item->title() );
    }
  }
  myInfLabel->setText( infoText );
}

void QtxPagePrefListItem::updateState()
{
  QtxPagePrefItem* item = selectedItem();
  QWidget* wid = item && !item->isEmpty() ? item->widget() : myInfLabel;
  if ( wid )
    myStack->setCurrentWidget( wid );

  updateInfo();
}

void QtxPagePrefListItem::updateVisible()
{
  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  QMap<QWidget*, int> map;
  for ( int i = 0; i < (int)myStack->count(); i++ )
    map.insert( myStack->widget( i ), 0 );

  int selId = selected();
  myList->clear();
  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( (*it)->isEmpty() && myInfText.isEmpty() )
      continue;

    myList->addItem( (*it)->title() );
    myList->item( myList->count() - 1 )->setIcon( (*it)->icon() );
    myList->item( myList->count() - 1 )->setData( Qt::UserRole, (*it)->id() );

    QWidget* wid = (*it)->widget();
    if ( !map.contains( wid ) )
      myStack->addWidget( wid );

    map.remove( wid );
  }

  map.remove( myInfLabel );

  for ( QMap<QWidget*, int>::const_iterator it = map.begin(); it != map.end(); ++it )
    myStack->removeWidget( it.key() );

  setSelected( selId );
  if ( selected() == -1 && myList->count() )
    setSelected( myList->item( 0 )->data( Qt::UserRole ).toInt() );

  myList->setVisible( myList->count() > 1 );

  updateState();
  updateGeom();
}

void QtxPagePrefListItem::updateGeom()
{
  if ( myFix )
    myList->setFixedWidth( myList->minimumSizeHint().width() + 10 );
  else
  {
    myList->setMinimumWidth( 0 );
    myList->setMaximumWidth( 16777215 );

    QSplitter* s = ::qobject_cast<QSplitter*>( widget() );
    if ( s )
    {
      int w = myList->minimumSizeHint().width() + 30;
      QList<int> szList;
      szList.append( w );
      szList.append( s->width() - w );
      s->setSizes( szList );
    }
  }
}

int QtxPagePrefListItem::selected() const
{
  QList<QListWidgetItem*> selList = myList->selectedItems();
  if ( selList.isEmpty() )
    return -1;

  QVariant v = selList.first()->data( Qt::UserRole );
  return v.canConvert( QVariant::Int ) ? v.toInt() : -1;
}

QtxPagePrefItem* QtxPagePrefListItem::selectedItem() const
{
  int selId = selected();

  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  QtxPagePrefItem* item = 0;
  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end() && !item; ++it )
  {
    if ( (*it)->id() == selId )
      item = *it;
  }
  return item;
}

void QtxPagePrefListItem::setSelected( const int id )
{
  int idx = -1;
  for ( int i = 0; i < (int)myList->count() && idx < 0; i++ )
  {
    QVariant v = myList->item( i )->data( Qt::UserRole );
    if ( v.canConvert( QVariant::Int ) && v.toInt() == id )
      idx = i;
  }

  QItemSelection sel;
  QItemSelectionModel* selModel = myList->selectionModel();

  if ( idx >= 0 )
    sel.select( myList->model()->index( idx, 0 ), myList->model()->index( idx, 0 ) );

  selModel->select( sel, QItemSelectionModel::ClearAndSelect );
}

/*!
  \class QtxPagePrefTabsItem
*/

QtxPagePrefTabsItem::QtxPagePrefTabsItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param )
{
  myTabs = new QTabWidget( 0 );
  setWidget( myTabs );
}

QtxPagePrefTabsItem::~QtxPagePrefTabsItem()
{
}

void QtxPagePrefTabsItem::updateContents()
{
  updateTabs();
}

int QtxPagePrefTabsItem::tabPosition() const
{
  return myTabs->tabPosition();
}

void QtxPagePrefTabsItem::setTabPosition( const int tp )
{
  myTabs->setTabPosition( (QTabWidget::TabPosition)tp );
}

int QtxPagePrefTabsItem::tabShape() const
{
  return myTabs->tabShape();
}

void QtxPagePrefTabsItem::setTabShape( const int ts )
{
  myTabs->setTabShape( (QTabWidget::TabShape)ts );
}

QSize QtxPagePrefTabsItem::tabIconSize() const
{
  return myTabs->iconSize();
}

void QtxPagePrefTabsItem::setTabIconSize( const QSize& sz )
{
  myTabs->setIconSize( sz );
}

QVariant QtxPagePrefTabsItem::optionValue( const QString& name ) const
{
  if ( name == "position" )
    return tabPosition();
  else if ( name == "shape" )
    return tabShape();
  else if ( name == "icon_size" )
    return tabIconSize();
  else
    return QtxPagePrefItem::optionValue( name );
}

void QtxPagePrefTabsItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "position" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setTabPosition( val.toInt() );
  }
  else if ( name == "shape" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setTabShape( val.toInt() );
  }
  else if ( name == "icon_size" )
  {
    if ( val.canConvert( QVariant::Size ) )
      setTabIconSize( val.toSize() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

void QtxPagePrefTabsItem::updateTabs()
{
  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  QWidget* cur = myTabs->currentWidget();

  int i = 0;
  QMap<QWidget*, int> map;
  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QWidget* wid = (*it)->widget();
    if ( !wid )
      continue;

    if ( myTabs->widget( i ) != wid )
    {
      if ( myTabs->indexOf( wid ) != -1 )
        myTabs->removeTab( myTabs->indexOf( wid ) );

      myTabs->insertTab( i, wid, (*it)->title() );
    }
    else
      myTabs->setTabText( i, (*it)->title() );

    myTabs->setTabIcon( i, (*it)->icon() );

    i++;
    map.insert( wid, 0 );
  }

  QList<QWidget*> del;
  for ( int idx = 0; idx < (int)myTabs->count(); idx++ )
  {
    QWidget* w = myTabs->widget( idx );
    if ( !map.contains( w ) )
      del.append( w );
  }

  for ( QList<QWidget*>::const_iterator itr = del.begin(); itr != del.end(); ++itr )
    myTabs->removeTab( myTabs->indexOf( *itr ) );

  myTabs->setCurrentWidget( cur );
}

/*!
  \class QtxPagePrefFrameItem
*/

QtxPagePrefFrameItem::QtxPagePrefFrameItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param )
{
  myBox = new QtxGridBox( 1, Qt::Horizontal, 0, 5, 0 );
  setWidget( myBox );
}

QtxPagePrefFrameItem::~QtxPagePrefFrameItem()
{
}

void QtxPagePrefFrameItem::updateContents()
{
  updateFrame();
}

int QtxPagePrefFrameItem::margin() const
{
  return myBox->insideMargin();
}

void QtxPagePrefFrameItem::setMargin( const int m )
{
  myBox->setInsideMargin( m );
}

int QtxPagePrefFrameItem::spacing() const
{
  return myBox->insideSpacing();
}

void QtxPagePrefFrameItem::setSpacing( const int s )
{
  myBox->setInsideSpacing( s );
}

int QtxPagePrefFrameItem::columns() const
{
  return myBox->columns();
}

void QtxPagePrefFrameItem::setColumns( const int c )
{
  myBox->setColumns( c );
}

Qt::Orientation QtxPagePrefFrameItem::orientation() const
{
  return myBox->orientation();
}

void QtxPagePrefFrameItem::setOrientation( const Qt::Orientation o )
{
  myBox->setOrientation( o );
}

QVariant QtxPagePrefFrameItem::optionValue( const QString& name ) const
{
  if ( name == "margin" )
    return margin();
  else if ( name == "spacing" )
    return spacing();
  else if ( name == "columns" )
    return columns();
  else if ( name == "orientation" )
    return orientation();
  else
    return QtxPagePrefItem::optionValue( name );
}

void QtxPagePrefFrameItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "margin" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setMargin( val.toInt() );
  }
  else if ( name == "spacing" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setSpacing( val.toInt() );
  }
  else if ( name == "columns" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setColumns( val.toInt() );
  }
  else if ( name == "orientation" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setOrientation( (Qt::Orientation)val.toInt() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

void QtxPagePrefFrameItem::updateFrame()
{
  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QWidget* wid = (*it)->widget();
    if ( !wid )
      continue;

    if ( wid->parent() != myBox )
      wid->setParent( myBox );
  }
}

/*!
  \class QtxPagePrefGroupItem
*/

QtxPagePrefGroupItem::QtxPagePrefGroupItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param )
{
  myGroup = new QtxGroupBox( title, 0 );
  myBox = new QtxGridBox( 1, Qt::Horizontal, myGroup, 5, 5 );
  myGroup->setWidget( myBox );

  setWidget( myGroup );
}

QtxPagePrefGroupItem::QtxPagePrefGroupItem( const int cols, const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param )
{
  myGroup = new QtxGroupBox( title, 0 );
  myBox = new QtxGridBox( cols, Qt::Horizontal, myGroup, 5, 5 );
  myGroup->setWidget( myBox );

  setWidget( myGroup );
}

QtxPagePrefGroupItem::~QtxPagePrefGroupItem()
{
}

void QtxPagePrefGroupItem::setResource( const QString& sect, const QString& param )
{
  QtxPagePrefItem::setResource( sect, param );
  updateState();
}

void QtxPagePrefGroupItem::updateContents()
{
  myGroup->setTitle( title() );

  updateState();
  updateGroup();
}

int QtxPagePrefGroupItem::margin() const
{
  return myBox->insideMargin();
}

void QtxPagePrefGroupItem::setMargin( const int m )
{
  myBox->setInsideMargin( m );
}

int QtxPagePrefGroupItem::spacing() const
{
  return myBox->insideSpacing();
}

void QtxPagePrefGroupItem::setSpacing( const int s )
{
  myBox->setInsideSpacing( s );
}

int QtxPagePrefGroupItem::columns() const
{
  return myBox->columns();
}

void QtxPagePrefGroupItem::setColumns( const int c )
{
  myBox->setColumns( c );
}

Qt::Orientation QtxPagePrefGroupItem::orientation() const
{
  return myBox->orientation();
}

void QtxPagePrefGroupItem::setOrientation( const Qt::Orientation o )
{
  myBox->setOrientation( o );
}

bool QtxPagePrefGroupItem::isFlat() const
{
  return myGroup->isFlat();
}

void QtxPagePrefGroupItem::setFlat( const bool on )
{
  myGroup->setFlat( on );
}

void QtxPagePrefGroupItem::store()
{
  if ( myGroup->isCheckable() )
    setBoolean( myGroup->isChecked() );
}

void QtxPagePrefGroupItem::retrieve()
{
  if ( myGroup->isCheckable() )
    myGroup->setChecked( getBoolean() );
}

QVariant QtxPagePrefGroupItem::optionValue( const QString& name ) const
{
  if ( name == "margin" )
    return margin();
  else if ( name == "spacing" )
    return spacing();
  else if ( name == "columns" )
    return columns();
  else if ( name == "orientation" )
    return orientation();
  else if ( name == "flat" )
    return isFlat();
  else
    return QtxPagePrefItem::optionValue( name );
}

void QtxPagePrefGroupItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "margin" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setMargin( val.toInt() );
  }
  else if ( name == "spacing" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setSpacing( val.toInt() );
  }
  else if ( name == "columns" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setColumns( val.toInt() );
  }
  else if ( name == "orientation" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setOrientation( (Qt::Orientation)val.toInt() );
  }
  else if ( name == "flat" )
  {
    if ( val.canConvert( QVariant::Bool ) )
      setFlat( val.toBool() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

void QtxPagePrefGroupItem::updateState()
{
  QString section, param;
  resource( section, param );
  myGroup->setCheckable( !title().isEmpty() && !section.isEmpty() && !param.isEmpty() );
}

void QtxPagePrefGroupItem::updateGroup()
{
  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QWidget* wid = (*it)->widget();
    if ( !wid )
      continue;

    if ( wid->parent() != myBox )
      wid->setParent( myBox );
  }
}

/*!
  \class QtxPagePrefSpaceItem
*/

QtxPagePrefSpaceItem::QtxPagePrefSpaceItem( QtxPreferenceItem* parent )
: QtxPagePrefItem( QString(), parent )
{
  initialize( 0, 0, 1, 1 );
}

QtxPagePrefSpaceItem::QtxPagePrefSpaceItem( Qt::Orientation o, QtxPreferenceItem* parent )
: QtxPagePrefItem( QString(), parent )
{
  if ( o == Qt::Horizontal )
    initialize( 0, 0, 1, 0 );
  else
    initialize( 0, 0, 0, 1 );
}

QtxPagePrefSpaceItem::QtxPagePrefSpaceItem( const int w, const int h, QtxPreferenceItem* parent )
: QtxPagePrefItem( QString(), parent )
{
  initialize( w, h, w > 0 ? 0 : 1, h > 0 ? 0 : 1 );
}

QtxPagePrefSpaceItem::~QtxPagePrefSpaceItem()
{
}

int QtxPagePrefSpaceItem::size( Qt::Orientation o ) const
{
  return o == Qt::Horizontal ? widget()->minimumWidth() : widget()->minimumHeight();
}

void QtxPagePrefSpaceItem::setSize( Qt::Orientation o, const int sz )
{
  if ( o == Qt::Horizontal )
    widget()->setMinimumWidth( sz );
  else
    widget()->setMinimumHeight( sz );
}

int QtxPagePrefSpaceItem::stretch( Qt::Orientation o ) const
{
  QSizePolicy sp = widget()->sizePolicy();
  return o == Qt::Horizontal ? sp.horizontalStretch() : sp.verticalStretch();
}

void QtxPagePrefSpaceItem::setStretch( Qt::Orientation o, const int sf )
{
  QSizePolicy sp = widget()->sizePolicy();
  if ( o == Qt::Horizontal )
  {
    sp.setHorizontalStretch( sf );
    sp.setHorizontalPolicy( sf > 0 ? QSizePolicy::Expanding : QSizePolicy::Fixed );
  }
  else
  {
    sp.setVerticalStretch( sf );
    sp.setVerticalPolicy( sf > 0 ? QSizePolicy::Expanding : QSizePolicy::Fixed );
  }
    
  widget()->setSizePolicy( sp );
}

QVariant QtxPagePrefSpaceItem::optionValue( const QString& name ) const
{
  if ( name == "horizontal_size" || name == "hsize" )
    return size( Qt::Horizontal );
  else if ( name == "vertical_size" || name == "vsize" )
    return size( Qt::Vertical );
  else if ( name == "horizontal_stretch" || name == "hstretch" )
    return stretch( Qt::Horizontal );
  else if ( name == "vertical_stretch" || name == "vstretch" )
    return stretch( Qt::Vertical );
  else
    return QtxPagePrefItem::optionValue( name );
}

void QtxPagePrefSpaceItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "horizontal_size" || name == "hsize" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setSize( Qt::Horizontal, val.toInt() );
  }
  else if ( name == "vertical_size" || name == "vsize" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setSize( Qt::Vertical, val.toInt() );
  }
  else if ( name == "horizontal_stretch" || name == "hstretch" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setStretch( Qt::Horizontal, val.toInt() );
  }
  else if ( name == "vertical_stretch" || name == "vstretch" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setStretch( Qt::Vertical, val.toInt() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

void QtxPagePrefSpaceItem::initialize( const int w, const int h, const int hs, const int vs )
{
  QSizePolicy sp;
  sp.setHorizontalPolicy( hs > 0 ? QSizePolicy::Expanding : QSizePolicy::Fixed );
  sp.setVerticalPolicy( vs > 0 ? QSizePolicy::Expanding : QSizePolicy::Fixed );
  
  sp.setHorizontalStretch( hs );
  sp.setVerticalStretch( vs );

  QWidget* wid = new QWidget();
  wid->setSizePolicy( sp );

  wid->setMinimumSize( w, h );
  
  setWidget( wid );
}

/*!
  \class  QtxPagePrefCheckItem
  GUI implementation of resources check item (bool).
*/

QtxPagePrefCheckItem::QtxPagePrefCheckItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )

: QtxPagePrefItem( title, parent, sect, param )
{
  myCheck = new QCheckBox( title );
  myCheck->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  setWidget( myCheck );
}

QtxPagePrefCheckItem::~QtxPagePrefCheckItem()
{
}

void QtxPagePrefCheckItem::setTitle( const QString& txt )
{
  QtxPagePrefItem::setTitle( txt );

  myCheck->setText( title() );
}
                                            
void QtxPagePrefCheckItem::store()
{
  setBoolean( myCheck->isChecked() );
}

void QtxPagePrefCheckItem::retrieve()
{
  myCheck->setChecked( getBoolean() );
}

/*!
  \class  QtxPagePrefEditItem
  GUI implementation of resources line edit item (string, integer, double).
*/

QtxPagePrefEditItem::QtxPagePrefEditItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
myType( String )
{
  setControl( myEditor = new QLineEdit() );
  updateEditor();
}

QtxPagePrefEditItem::QtxPagePrefEditItem( const int type, const QString& title,
                                          QtxPreferenceItem* parent, const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
myType( type )
{
  setControl( myEditor = new QLineEdit() );
  updateEditor();
}

QtxPagePrefEditItem::~QtxPagePrefEditItem()
{
}

int QtxPagePrefEditItem::inputType() const
{
  return myType;
}

void QtxPagePrefEditItem::setInputType( const int type )
{
  if ( myType == type )
    return;

  myType = type;
  updateEditor();
}

void QtxPagePrefEditItem::store()
{
  setString( myEditor->text() );
}

void QtxPagePrefEditItem::retrieve()
{
  QString txt = getString();
  if ( myEditor->validator() )
  {
    int pos = 0;
    if ( myEditor->validator()->validate( txt, pos ) == QValidator::Invalid )
      txt.clear();
  }
  myEditor->setText( txt );
}

QVariant QtxPagePrefEditItem::optionValue( const QString& name ) const
{
  if ( name == "input_type" || name == "type" )
    return inputType();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

void QtxPagePrefEditItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "input_type" || name == "type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setInputType( val.toInt() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

void QtxPagePrefEditItem::updateEditor()
{
  QValidator* val = 0;
  switch ( inputType() )
  {
  case Integer:
    val = new QIntValidator( myEditor );
    break;
  case Double:
    val = new QDoubleValidator( myEditor );
    break;
  default:
    break;
  }

  if ( !myEditor->text().isEmpty() && val )
  {
    int pos = 0;
    QString str = myEditor->text();
    if ( val->validate( str, pos ) == QValidator::Invalid )
      myEditor->clear();
  }

  delete myEditor->validator();
  myEditor->setValidator( val );
}

/*!
  \class QtxPagePrefSelectItem
  GUI implementation of resources selector item (enum).
*/
QtxPagePrefSelectItem::QtxPagePrefSelectItem( const QString& title, QtxPreferenceItem* parent,
                                              const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
myType( NoInput )
{
  setControl( mySelector = new QtxComboBox() );
  mySelector->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  mySelector->setDuplicatesEnabled( false );
  updateSelector();
}

QtxPagePrefSelectItem::QtxPagePrefSelectItem( const int type, const QString& title, QtxPreferenceItem* parent,
                                              const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
myType( type )
{
  setControl( mySelector = new QtxComboBox() );
  mySelector->setDuplicatesEnabled( false );
  updateSelector();
}

QtxPagePrefSelectItem::~QtxPagePrefSelectItem()
{
}

int QtxPagePrefSelectItem::inputType() const
{
  return myType;
}

void QtxPagePrefSelectItem::setInputType( const int type )
{
  if ( myType == type )
    return;

  myType = type;
  updateSelector();
}

QStringList QtxPagePrefSelectItem::strings() const
{
  QStringList res;
  for ( uint i = 0; i < mySelector->count(); i++ )
    res.append( mySelector->itemText( i ) );
  return res;
}

QList<int> QtxPagePrefSelectItem::numbers() const
{
  QList<int> res;
  for ( uint i = 0; i < mySelector->count(); i++ )
  {
    if ( mySelector->hasId( i ) )
      res.append( mySelector->id( i ) );
  }
  return res;
}

void QtxPagePrefSelectItem::setStrings( const QStringList& lst )
{
  mySelector->clear();
  mySelector->addItems( lst );
}

void QtxPagePrefSelectItem::setNumbers( const QList<int>& ids )
{
  uint i = 0;
  for ( QList<int>::const_iterator it = ids.begin(); it != ids.end() && i < mySelector->count(); ++it, i++ )
    mySelector->setId( i, *it );
}

void QtxPagePrefSelectItem::store()
{
  if ( mySelector->isCleared() )
    return;

  int idx = mySelector->currentIndex();

  if ( mySelector->hasId( idx ) )
    setInteger( mySelector->id( idx ) );
  else if ( idx >= 0 )
    setString( mySelector->itemText( idx ) );
}

void QtxPagePrefSelectItem::retrieve()
{
  QString txt = getString();

  int idx = -1;

  bool ok = false;
  int num = txt.toInt( &ok );
  if ( ok )
    idx = mySelector->index( num );
  else
  {
    for ( uint i = 0; i < mySelector->count() && idx == -1; i++ )
    {
      if ( mySelector->itemText( i ) == txt )
        idx = i;
    }
  }

  if ( idx != -1 )
    mySelector->setCurrentIndex( idx );
  else if ( mySelector->isEditable() )
  {
    int pos = 0;
    if ( mySelector->validator() &&
         mySelector->validator()->validate( txt, pos ) == QValidator::Invalid )
      mySelector->setCleared( true );
    else
    {
      mySelector->setCleared( false );
      mySelector->addItem( txt );
      mySelector->setCurrentIndex( mySelector->count() - 1 );
    }
  }
}

QVariant QtxPagePrefSelectItem::optionValue( const QString& name ) const
{
  if ( name == "input_type" || name == "type" )
    return inputType();
  else if ( name == "strings" || name == "labels" )
    return strings();
  else if ( name == "numbers" || name == "ids" || name == "indexes" )
  {
    QList<QVariant> lst;
    QList<int> nums = numbers();
    for ( QList<int>::const_iterator it = nums.begin(); it != nums.end(); ++it )
      lst.append( *it );
    return lst;
  }
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

void QtxPagePrefSelectItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "input_type" || name == "type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setInputType( val.toInt() );
  }
  else if ( name == "strings" || name == "labels" )
    setStrings( val );
  else if ( name == "numbers" || name == "ids" || name == "indexes" )
    setNumbers( val );
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

void QtxPagePrefSelectItem::setStrings( const QVariant& var )
{
  if ( var.type() != QVariant::StringList )
    return;

  setStrings( var.toStringList() );
}

void QtxPagePrefSelectItem::setNumbers( const QVariant& var )
{
  if ( var.type() != QVariant::List )
    return;

  QList<int> lst;
  QList<QVariant> varList = var.toList();
  for ( QList<QVariant>::const_iterator it = varList.begin(); it != varList.end(); ++it )
  {
    if ( (*it).canConvert( QVariant::Int ) )
      lst.append( (*it).toInt() );
  }
  setNumbers( lst );
}

void QtxPagePrefSelectItem::updateSelector()
{
  QValidator* val = 0;
  switch ( inputType() )
  {
  case Integer:
    val = new QIntValidator( mySelector );
    break;
  case Double:
    val = new QDoubleValidator( mySelector );
    break;
  default:
    break;
  }

  mySelector->setEditable( inputType() != NoInput );

  if ( mySelector->isEditable() && !mySelector->currentText().isEmpty() && val )
  {
    int pos = 0;
    QString str = mySelector->currentText();
    if ( val->validate( str, pos ) == QValidator::Invalid )
      mySelector->clearEditText();
  }

  delete mySelector->validator();
  mySelector->setValidator( val );
}

/*!
  \class  QtxPagePrefSpinItem
  GUI implementation of resources spin box item (integer, double).
*/

QtxPagePrefSpinItem::QtxPagePrefSpinItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
myType( Integer )
{
  updateSpinBox();
}

QtxPagePrefSpinItem::QtxPagePrefSpinItem( const int type, const QString& title,
                                          QtxPreferenceItem* parent, const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
myType( type )
{
  updateSpinBox();
}

QtxPagePrefSpinItem::~QtxPagePrefSpinItem()
{
}

int QtxPagePrefSpinItem::inputType() const
{
  return myType;
}

void QtxPagePrefSpinItem::setInputType( const int type )
{
  if ( myType == type )
    return;

  myType = type;
  updateSpinBox();
}

QVariant QtxPagePrefSpinItem::step() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->singleStep();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->singleStep();
  else
    return QVariant();
}

QVariant QtxPagePrefSpinItem::minimum() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->minimum();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->minimum();
  else
    return QVariant();
}

QVariant QtxPagePrefSpinItem::maximum() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->maximum();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->maximum();
  else
    return QVariant();
}

QString QtxPagePrefSpinItem::prefix() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->prefix();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->prefix();
  else
    return QString();
}

QString QtxPagePrefSpinItem::suffix() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->suffix();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->suffix();
  else
    return QString();
}

QString QtxPagePrefSpinItem::specialValueText() const
{
  QAbstractSpinBox* sb = ::qobject_cast<QAbstractSpinBox*>( control() );
  if ( sb )
    return sb->specialValueText();
  else
    return QString();
}

void QtxPagePrefSpinItem::setStep( const QVariant& step )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
  {
    if ( step.canConvert( QVariant::Int ) )
      isb->setSingleStep( step.toInt() );
  }
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
  {
    if ( step.canConvert( QVariant::Double ) )
      dsb->setSingleStep( step.toDouble() );
  }
}

void QtxPagePrefSpinItem::setMinimum( const QVariant& min )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
  {
    if ( min.canConvert( QVariant::Int ) )
      isb->setMinimum( min.toInt() );
  }
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
  {
    if ( min.canConvert( QVariant::Double ) )
      dsb->setMinimum( min.toDouble() );
  }
}

void QtxPagePrefSpinItem::setMaximum( const QVariant& max )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
  {
    if ( max.canConvert( QVariant::Int ) )
      isb->setMaximum( max.toInt() );
  }
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
  {
    if ( max.canConvert( QVariant::Double ) )
      dsb->setMaximum( max.toDouble() );
  }
}

void QtxPagePrefSpinItem::setPrefix( const QString& txt )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    isb->setPrefix( txt );
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    dsb->setPrefix( txt );
}

void QtxPagePrefSpinItem::setSuffix( const QString& txt )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    isb->setSuffix( txt );
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    dsb->setSuffix( txt );
}

void QtxPagePrefSpinItem::setSpecialValueText( const QString& txt )
{
  QAbstractSpinBox* sb = ::qobject_cast<QAbstractSpinBox*>( control() );
  if ( sb )
    sb->setSpecialValueText( txt );
}

void QtxPagePrefSpinItem::store()
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    setInteger( isb->value() );
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    setDouble( dsb->value() );
}

void QtxPagePrefSpinItem::retrieve()
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    isb->setValue( getInteger( isb->value() ) );
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    dsb->setValue( getDouble( dsb->value() ) );
}

QVariant QtxPagePrefSpinItem::optionValue( const QString& name ) const
{
  if ( name == "input_type" || name == "type" )
    return inputType();
  else if ( name == "minimum" || name == "min" )
    return minimum();
  else if ( name == "maximum" || name == "max" )
    return maximum();
  else if ( name == "step" )
    return step();
  else if ( name == "prefix" )
    return prefix();
  else if ( name == "suffix" )
    return suffix();
  else if ( name == "special" )
    return specialValueText();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

void QtxPagePrefSpinItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "input_type" || name == "type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setInputType( val.toInt() );
  }
  else if ( name == "minimum" || name == "min" )
    setMinimum( val );
  else if ( name == "maximum" || name == "max" )
    setMaximum( val );
  else if ( name == "step" )
    setStep( val );
  else if ( name == "prefix" )
  {
    if ( val.canConvert( QVariant::String ) )
      setPrefix( val.toString() );
  }
  else if ( name == "suffix" )
  {
    if ( val.canConvert( QVariant::String ) )
      setSuffix( val.toString() );
  }
  else if ( name == "special" )
  {
    if ( val.canConvert( QVariant::String ) )
      setSpecialValueText( val.toString() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

void QtxPagePrefSpinItem::updateSpinBox()
{
  QVariant val;
  QVariant stp = step();
  QVariant min = minimum();
  QVariant max = maximum();

  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    val = isb->value();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    val = dsb->value();

  switch ( inputType() )
  {
  case Integer:
    setControl( new QtxIntSpinBox() );
    break;
  case Double:
    setControl( new QtxDoubleSpinBox() );
    break;
  default:
    break;
  }

  control()->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

  setStep( stp );
  setMinimum( min );
  setMaximum( max );

  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
  {
    if ( val.canConvert( QVariant::Int ) )
      isb->setValue( val.toInt() );
  }
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
  {
    if ( val.canConvert( QVariant::Double ) )
      dsb->setValue( val.toDouble() );
  }
}

/*!
  \class  QtxPagePrefTextItem
  GUI implementation of resources text edit item (text - several strings).
*/
QtxPagePrefTextItem::QtxPagePrefTextItem( QtxPreferenceItem* parent, const QString& sect, const QString& param )
: QtxPageNamedPrefItem( QString(), parent, sect, param )
{
  myEditor = new QTextEdit();
  myEditor->setAcceptRichText( false );

  setControl( myEditor );
}

QtxPagePrefTextItem::QtxPagePrefTextItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  myEditor = new QTextEdit();
  myEditor->setAcceptRichText( false );

  setControl( myEditor );
}

QtxPagePrefTextItem::~QtxPagePrefTextItem()
{
}

void QtxPagePrefTextItem::store()
{
  setString( myEditor->toPlainText() );
}

void QtxPagePrefTextItem::retrieve()
{
  myEditor->setPlainText( getString() );
}

/*!
  \class  QtxPagePrefColorItem
  GUI implementation of resources color item.
*/
QtxPagePrefColorItem::QtxPagePrefColorItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myColor = new QtxColorButton() );
  myColor->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
}

QtxPagePrefColorItem::~QtxPagePrefColorItem()
{
}

void QtxPagePrefColorItem::store()
{
  setColor( myColor->color() );
}

void QtxPagePrefColorItem::retrieve()
{
  myColor->setColor( getColor() );
}

/*!
  \class  QtxPagePrefFontItem
  GUI implementation of resources font item.
*/
QtxPagePrefFontItem::QtxPagePrefFontItem( const int feat, const QString& title,
                                          QtxPreferenceItem* parent, const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myFont = new QtxFontEdit( feat ) );
}

QtxPagePrefFontItem::QtxPagePrefFontItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myFont = new QtxFontEdit() );
}

QtxPagePrefFontItem::~QtxPagePrefFontItem()
{
}

int QtxPagePrefFontItem::features() const
{
  return myFont->features();
}

void QtxPagePrefFontItem::setFeatures( const int f )
{
  myFont->setFeatures( f );
}

void QtxPagePrefFontItem::store()
{
  setFont( myFont->currentFont() );
}

void QtxPagePrefFontItem::retrieve()
{
  myFont->setCurrentFont( getFont() );
}

QVariant QtxPagePrefFontItem::optionValue( const QString& name ) const
{
  if ( name == "features" )
    return features();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

void QtxPagePrefFontItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "features" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setFeatures( val.toInt() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \class  QtxPagePrefPathItem
  GUI implementation of resources path item.
*/
QtxPagePrefPathItem::QtxPagePrefPathItem( const Qtx::PathType type, const QString& title,
                                          QtxPreferenceItem* parent, const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myPath = new QtxPathEdit( type ) );
}

QtxPagePrefPathItem::QtxPagePrefPathItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myPath = new QtxPathEdit() );
}

QtxPagePrefPathItem::~QtxPagePrefPathItem()
{
}

Qtx::PathType QtxPagePrefPathItem::pathType() const
{
  return myPath->pathType();
}

void QtxPagePrefPathItem::setPathType( const Qtx::PathType type )
{
  myPath->setPathType( type );
}

QString QtxPagePrefPathItem::pathFilter() const
{
  return myPath->pathFilter();
}

void QtxPagePrefPathItem::setPathFilter( const QString& f )
{
  myPath->setPathFilter( f );
}

void QtxPagePrefPathItem::store()
{
  setString( myPath->path() );
}

void QtxPagePrefPathItem::retrieve()
{
  myPath->setPath( getString() );
}

QVariant QtxPagePrefPathItem::optionValue( const QString& name ) const
{
  if ( name == "path_type" )
    return pathType();
  else if ( name == "path_filter" )
    return pathFilter();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

void QtxPagePrefPathItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "path_type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setPathType( (Qtx::PathType)val.toInt() );
  }
  else if ( name == "path_filter" )
  {
    if ( val.canConvert( QVariant::String ) )
      setPathFilter( val.toString() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \class  QtxPagePrefPathsItem
  \brief GUI implementation of resources directory list item.
*/
QtxPagePrefPathsItem::QtxPagePrefPathsItem( QtxPreferenceItem* parent, const QString& sect, const QString& param )
: QtxPageNamedPrefItem( QString(), parent, sect, param )
{
  setControl( myPaths = new QtxPathListEdit() );
}

QtxPagePrefPathsItem::QtxPagePrefPathsItem( const Qtx::PathType type, const QString& title,
                                            QtxPreferenceItem* parent, const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myPaths = new QtxPathListEdit( type ) );
}

QtxPagePrefPathsItem::QtxPagePrefPathsItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myPaths = new QtxPathListEdit() );
}

QtxPagePrefPathsItem::~QtxPagePrefPathsItem()
{
}

Qtx::PathType QtxPagePrefPathsItem::pathType() const
{
  return myPaths->pathType();
}

void QtxPagePrefPathsItem::setPathType( const Qtx::PathType type )
{
  myPaths->setPathType( type );
}

void QtxPagePrefPathsItem::store()
{
  setString( myPaths->pathList().join( ";" ) );
}

void QtxPagePrefPathsItem::retrieve()
{
  myPaths->setPathList( getString().split( ";" ) );
}

QVariant QtxPagePrefPathsItem::optionValue( const QString& name ) const
{
  if ( name == "path_type" )
    return pathType();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

void QtxPagePrefPathsItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "path_type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setPathType( (Qtx::PathType)val.toInt() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}
