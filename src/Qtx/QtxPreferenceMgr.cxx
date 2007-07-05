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
// File:      QtxPreferenceMgr.cxx
// Author:    Sergey TELKOV

#include "QtxPreferenceMgr.h"

#include "QtxResourceMgr.h"

#include <QEvent>
#include <QApplication>

/*!
  \class QtxPreferenceItem::Updater
  Class for incapsulation of one preference item
*/

class QtxPreferenceItem::Updater : public QObject
{
public:
  Updater();
  ~Updater();

  static Updater*           instance();

  void                      updateItem( QtxPreferenceItem* );
  void                      removeItem( QtxPreferenceItem* );

protected:
  virtual void              customEvent( QEvent* );

private:
  QList<QtxPreferenceItem*> myItems;
  static Updater*           _Updater;
};

QtxPreferenceItem::Updater* QtxPreferenceItem::Updater::_Updater = 0;

QtxPreferenceItem::Updater::Updater()
{
}

QtxPreferenceItem::Updater::~Updater()
{
}

QtxPreferenceItem::Updater* QtxPreferenceItem::Updater::instance()
{
  if ( !_Updater )
    _Updater = new Updater();
  return _Updater;
}

void QtxPreferenceItem::Updater::updateItem( QtxPreferenceItem* item )
{
  if ( !item || myItems.contains( item ) )
    return;

  myItems.append( item );
  QApplication::postEvent( this, new QEvent( QEvent::User ) );
}

void QtxPreferenceItem::Updater::removeItem( QtxPreferenceItem* item )
{
  myItems.removeAll( item );
}

void QtxPreferenceItem::Updater::customEvent( QEvent* e )
{
  QList<QtxPreferenceItem*> lst = myItems;
  for ( QList<QtxPreferenceItem*>::const_iterator it = lst.begin(); it != lst.end(); ++it )
    (*it)->updateContents();
}

/*!
  \class QtxPreferenceItem
  Class for incapsulation of one preference item
*/


/*!
  Constructor
*/
QtxPreferenceItem::QtxPreferenceItem( QtxPreferenceItem* parent )
: myParent( 0 )
{
  myId = generateId();

  if ( parent )
    parent->insertItem( this );
}

QtxPreferenceItem::QtxPreferenceItem( const QString& title, QtxPreferenceItem* parent )
: myParent( 0 ),
myTitle( title )
{
  myId = generateId();

  if ( parent )
    parent->insertItem( this );
}

QtxPreferenceItem::QtxPreferenceItem( const QString& title, const QString& sect,
                                      const QString& param, QtxPreferenceItem* parent )
: myParent( 0 ),
myTitle( title ),
mySection( sect ),
myParameter( param )
{
  myId = generateId();

  if ( parent )
    parent->insertItem( this );
}

/*!
  Destructor
*/
QtxPreferenceItem::~QtxPreferenceItem()
{
  ItemList list = myChildren;
  myChildren.clear();
  qDeleteAll( list );

  if ( myParent )
    myParent->removeItem( this );

  Updater::instance()->removeItem( this );
}

/*!
  \return id of item
*/
int QtxPreferenceItem::id() const
{
  return myId;
}

int QtxPreferenceItem::rtti() const
{
  return QtxPreferenceItem::RTTI();
}

int QtxPreferenceItem::RTTI()
{
  return 1;
}

/*!
  \return root item
*/
QtxPreferenceItem* QtxPreferenceItem::rootItem() const
{
  QtxPreferenceItem* item = (QtxPreferenceItem*)this;
  while ( item->parentItem() )
    item = item->parentItem();
  return item;
}

/*!
  \return parent item
*/
QtxPreferenceItem* QtxPreferenceItem::parentItem() const
{
  return myParent;
}

/*!
  Appends child and (if necessary) removes item from old parent
  \param item - item to be added
*/
void QtxPreferenceItem::insertItem( QtxPreferenceItem* item )
{
  if ( !item || myChildren.contains( item ) )
    return;

  if ( item->parentItem() && item->parentItem() != this )
    item->parentItem()->removeItem( item );

  item->myParent = this;
  myChildren.append( item );

  itemAdded( item );
}

/*!
  Removes child
  \param item - item to be removed
*/
void QtxPreferenceItem::removeItem( QtxPreferenceItem* item )
{
  if ( !item || !myChildren.contains( item ) )
    return;

  item->myParent = 0;
  myChildren.removeAll( item );

  itemRemoved( item );
}

/*!
  Fills list with children items
  \param lst - list to be filled with
*/
QList<QtxPreferenceItem*> QtxPreferenceItem::childItems( const bool rec ) const
{
  QList<QtxPreferenceItem*> lst = myChildren;
  if ( rec )
  {
    for ( ItemList::const_iterator it = myChildren.begin(); it != myChildren.end(); ++it )
      lst += (*it)->childItems( rec );
  }

  return lst;
}

int QtxPreferenceItem::depth() const
{
  return parentItem() ? parentItem()->depth() + 1 : 0;
}

int QtxPreferenceItem::count() const
{
  return myChildren.count();
}

/*!
  \return true if there is no children of this item
*/
bool QtxPreferenceItem::isEmpty() const
{
  return myChildren.isEmpty();
}

/*!
  \return icon of item
*/
QIcon QtxPreferenceItem::icon() const
{
  return myIcon;
}

/*!
  \return title of item
*/
QString QtxPreferenceItem::title() const
{
  return myTitle;
}

/*!
  \return assigned resource placement
  \param sec - to return section
  \param param - to return param name
*/
void QtxPreferenceItem::resource( QString& sec, QString& param ) const
{
  sec = mySection;
  param = myParameter;
}

/*!
  Sets item icon
  \param ico - new item icon
*/
void QtxPreferenceItem::setIcon( const QIcon& ico )
{
  if ( myIcon.serialNumber() == ico.serialNumber() )
    return;

  myIcon = ico;
  sendItemChanges();
}

/*!
  Sets item title 
  \param title - new item title
*/
void QtxPreferenceItem::setTitle( const QString& title )
{
  if ( myTitle == title )
    return;

  myTitle = title;
  sendItemChanges();
}

/*!
  Assigns new resource to item
  \param sec - section
  \param sec - param name
*/
void QtxPreferenceItem::setResource( const QString& sec, const QString& param )
{
  mySection = sec;
  myParameter = param;
}

/*!
  Updates item (default implementation is empty)
*/
void QtxPreferenceItem::updateContents()
{
  Updater::instance()->removeItem( this );
}

/*!
  \return property value
*/
QVariant QtxPreferenceItem::option( const QString& name ) const
{
  return optionValue( name.toLower() );
}

/*!
  Sets property value
*/
void QtxPreferenceItem::setOption( const QString& name, const QVariant& val )
{
  QVariant old = optionValue( name.toLower() );
  setOptionValue( name.toLower(), val );
  if ( old != optionValue( name.toLower() ) )
    sendItemChanges();
}

/*!
  \return value of assigned resource
*/
QString QtxPreferenceItem::resourceValue() const
{
  return getString();
}

/*!
  Sets value of assigned resource
  \param val - new value
*/
void QtxPreferenceItem::setResourceValue( const QString& val )
{
  setString( val );
}

/*!
  \return corresponding resource manager
*/
QtxResourceMgr* QtxPreferenceItem::resourceMgr() const
{
  QtxPreferenceMgr* mgr = preferenceMgr();
  return mgr ? mgr->resourceMgr() : 0;
}

/*!
  \return corresponding resource edit
*/
QtxPreferenceMgr* QtxPreferenceItem::preferenceMgr() const
{
  return parentItem() ? parentItem()->preferenceMgr() : 0;
}

/*!
  \return other item
  \param id - other item id
*/
QtxPreferenceItem* QtxPreferenceItem::findItem( const int id, const bool rec ) const
{
  QtxPreferenceItem* item = 0;
  for ( ItemList::const_iterator it = myChildren.begin(); it != myChildren.end() && !item; ++it )
  {
    QtxPreferenceItem* i = *it;
    if ( i->id() == id )
      item = i;
    else if ( rec )
      item = i->findItem( id, rec );
  }
  return item;
}

/*!
  \return other item
  \param title - other item title
*/
QtxPreferenceItem* QtxPreferenceItem::findItem( const QString& title, const bool rec ) const
{
  QtxPreferenceItem* item = 0;
  for ( ItemList::const_iterator it = myChildren.begin(); it != myChildren.end() && !item; ++it )
  {
    QtxPreferenceItem* i = *it;
    if ( i->title() == title )
      item = i;
    else if ( rec )
      item = i->findItem( title, rec );
  }
  return item;
}

/*!
  \return other item
  \param title - other item title
  \param id - parent item id
*/
QtxPreferenceItem* QtxPreferenceItem::findItem( const QString& title, const int id, const bool rec ) const
{
  QtxPreferenceItem* item = 0;
  for ( ItemList::const_iterator it = myChildren.begin(); it != myChildren.end() && !item; ++it )
  {
    QtxPreferenceItem* i = *it;
    if ( i->title() == title && i->id() == id )
      item = i;
    else if ( rec )
      item = i->findItem( title, id, rec );
  }
  return item;
}


/*!
  \return integer value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
int QtxPreferenceItem::getInteger( const int val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->integerValue( mySection, myParameter, val ) : val;
}

/*!
  \return double value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
double QtxPreferenceItem::getDouble( const double val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->doubleValue( mySection, myParameter, val ) : val;
}

/*!
  \return boolean value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
bool QtxPreferenceItem::getBoolean( const bool val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->booleanValue( mySection, myParameter, val ) : val;
}

/*!
  \return string value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
QString QtxPreferenceItem::getString( const QString& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->stringValue( mySection, myParameter, val ) : val;
}

/*!
  \return color value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
QColor QtxPreferenceItem::getColor( const QColor& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->colorValue( mySection, myParameter, val ) : val;
}

/*!
  \return font value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
QFont QtxPreferenceItem::getFont( const QFont& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->fontValue( mySection, myParameter, val ) : val;
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxPreferenceItem::setInteger( const int val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxPreferenceItem::setDouble( const double val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxPreferenceItem::setBoolean( const bool val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxPreferenceItem::setString( const QString& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxPreferenceItem::setColor( const QColor& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxPreferenceItem::setFont( const QFont& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

void QtxPreferenceItem::itemAdded( QtxPreferenceItem* )
{
}

void QtxPreferenceItem::itemRemoved( QtxPreferenceItem* )
{
}

void QtxPreferenceItem::itemChanged( QtxPreferenceItem* )
{
}

void QtxPreferenceItem::triggerUpdate()
{
  Updater::instance()->updateItem( this );
}

QVariant QtxPreferenceItem::optionValue( const QString& ) const
{
  return QVariant();
}

void QtxPreferenceItem::setOptionValue( const QString&, const QVariant& )
{
}

void QtxPreferenceItem::sendItemChanges()
{
  if ( parentItem() )
    parentItem()->itemChanged( this );
}

/*!
  \return free item id
*/
int QtxPreferenceItem::generateId()
{
  static int _id = 0;
  return _id++;
}



/*!
  \class QtxPreferenceMgr
  Class for managing preferences items
*/



/*!
  Constructor
*/
QtxPreferenceMgr::QtxPreferenceMgr( QtxResourceMgr* mgr )
: QtxPreferenceItem( 0 ),
myResMgr( mgr )
{
}

/*!
  Destructor
*/
QtxPreferenceMgr::~QtxPreferenceMgr()
{
}

/*!
  \return assigned resource manager
*/
QtxResourceMgr* QtxPreferenceMgr::resourceMgr() const
{
  return myResMgr;
}

QtxPreferenceMgr* QtxPreferenceMgr::preferenceMgr() const
{
  return (QtxPreferenceMgr*)this;
}

/*!
  Adds new item
  \param label - label of widget to edit preference
  \param pId - parent item id
  \param type - type of item
  \param section - section of resource assigned with item
  \param param - name of resource assigned with item
*/
/*
int QtxPreferenceMgr::addItem( const QString& label, const int pId, const int type,
                               const QString& section, const QString& param )
{
  Item* i = createItem( label, type, pId );
  if ( !i )
    return -1;

  if ( !myItems.contains( i->id() ) )
  {
    myItems.insert( i->id(), i );

    i->setTitle( label );
    i->setResource( section, param );

    if ( !i->parentItem() && !myChildren.contains( i ) )
      myChildren.append( i );

    itemAdded( i );
  }

  return i->id();
}
*/

/*!
  \return value of item property
  \param id - item id
  \propName - propertyName
*/
QVariant QtxPreferenceMgr::option( const int id, const QString& propName ) const
{
  QVariant propValue;
  QtxPreferenceItem* i = findItem( id, true );
  if ( i )
    propValue = i->option( propName );
  return propValue;
}
/*!
  Sets value of item property
  \param id - item id
  \propName - propertyName
  \propValue - new value of property
*/
void QtxPreferenceMgr::setOption( const int id, const QString& propName, const QVariant& propValue )
{
  QtxPreferenceItem* i = findItem( id, true );
  if ( i )
    i->setOption( propName, propValue );
}

/*!
  Stores all values to resource manager
*/
void QtxPreferenceMgr::store()
{
  ResourceMap before;
  resourceValues( before );

  QList<QtxPreferenceItem*> items = childItems( true );
  for ( QList<QtxPreferenceItem*>::iterator it = items.begin(); it != items.end(); ++it )
    (*it)->store();

  ResourceMap after;
  resourceValues( after );

  ResourceMap changed;
  differentValues( before, after, changed );

  changedResources( changed );
}

/*!
  Retrieve all values from resource manager
*/
void QtxPreferenceMgr::retrieve()
{
  QList<QtxPreferenceItem*> items = childItems( true );
  for ( QList<QtxPreferenceItem*>::iterator it = items.begin(); it != items.end(); ++it )
    (*it)->retrieve();
}

/*!
  Stores all values to backup container
*/
void QtxPreferenceMgr::toBackup()
{
  myBackup.clear();
  resourceValues( myBackup );
}

/*!
  Retrieve all values from backup container
*/
void QtxPreferenceMgr::fromBackup()
{
  ResourceMap before;
  resourceValues( before );

  setResourceValues( myBackup );

  ResourceMap after;
  resourceValues( after );

  ResourceMap changed;
  differentValues( before, after, changed );

  changedResources( changed );
}

/*!
  Updates resource edit (default implementation is empty)
*/
void QtxPreferenceMgr::update()
{
}

/*!
  Creates item
  \return new item
  \param label - text of label for new item
  \param type - type of new item
  \param pId - parent id

QtxPreferenceItem* QtxPreferenceMgr::createItem( const QString& label, const int type, const int pId )
{
  Item* i = 0;
  if ( pId < 0 )
    i = createItem( label, type );
  else
  {
    Item* pItem = item( pId );
    if ( pItem )
    {
      i = pItem->createItem( label, type );
      pItem->insertChild( i );
    }
  }

  return i;
}
*/

/*!
  \return all resources values from widgets
  \param map - map to be filled by resources values
*/
void QtxPreferenceMgr::resourceValues( QMap<int, QString>& map ) const
{
  QString sect, name;
  QtxResourceMgr* resMgr = resourceMgr();
  QList<QtxPreferenceItem*> items = childItems( true );
  for ( QList<QtxPreferenceItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QtxPreferenceItem* item = *it;
    item->resource( sect, name );
    if ( resMgr->hasValue( sect, name ) )
      map.insert( item->id(), item->resourceValue() );
  }
}

/*!
  \return all resources values from widgets
  \param map - map to be filled by resources values
*/
void QtxPreferenceMgr::resourceValues( ResourceMap& map ) const
{
  QString sect, name;
  QtxResourceMgr* resMgr = resourceMgr();
  QList<QtxPreferenceItem*> items = childItems( true );
  for ( QList<QtxPreferenceItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QtxPreferenceItem* item = *it;
    item->resource( sect, name );
    if ( resMgr->hasValue( sect, name ) )
      map.insert( item, item->resourceValue() );
  }
}

/*!
  Sets to widgets all resources values from map
  \param map - map with resources values
*/
void QtxPreferenceMgr::setResourceValues( QMap<int, QString>& map ) const
{
  for ( QMap<int, QString>::const_iterator it = map.begin(); it != map.end(); ++it )
  {
    QtxPreferenceItem* i = findItem( it.key(), true );
    if ( i )
      i->setResourceValue( it.value() );
  }
}

/*!
  Sets to widgets all resources values from map
  \param map - map with resources values
*/
void QtxPreferenceMgr::setResourceValues( ResourceMap& map ) const
{
  for ( ResourceMap::const_iterator it = map.begin(); it != map.end(); ++it )
    it.key()->setResourceValue( it.value() );
}

/*!
   Compares two map of resources values and finds different ones
   \param map1 - first map
   \param map2 - second map
   \param resMap - map to be filled with different values
   \param fromFirst - if it is true, then resMap will be filled with values from first map, otherwise - from second
*/
void QtxPreferenceMgr::differentValues( const QMap<int, QString>& map1, const QMap<int, QString>& map2,
                                        QMap<int, QString>& resMap, const bool fromFirst ) const
{
  resMap.clear();
  const QMap<int, QString>& later = fromFirst ? map1 : map2;
  const QMap<int, QString>& early = fromFirst ? map2 : map1;

  for ( QMap<int, QString>::const_iterator it = later.begin(); it != later.end(); ++it )
  {
    if ( !early.contains( it.key() ) || early[it.key()] != it.value() )
      resMap.insert( it.key(), it.value() );
  }
}

/*!
   Compares two map of resources values and finds different ones
   \param map1 - first map
   \param map2 - second map
   \param resMap - map to be filled with different values
   \param fromFirst - if it is true, then resMap will be filled with values from first map, otherwise - from second
*/
void QtxPreferenceMgr::differentValues( const ResourceMap& map1, const ResourceMap& map2,
                                        ResourceMap& resMap, const bool fromFirst ) const
{
  resMap.clear();
  const ResourceMap& later = fromFirst ? map1 : map2;
  const ResourceMap& early = fromFirst ? map2 : map1;

  for ( ResourceMap::const_iterator it = later.begin(); it != later.end(); ++it )
  {
    if ( !early.contains( it.key() ) || early[it.key()] != it.value() )
      resMap.insert( it.key(), it.value() );
  }
}

/*!
  Makes some activity on resource changing (called from store() method)
  \sa store()
*/
void QtxPreferenceMgr::changedResources( const ResourceMap& )
{
}
