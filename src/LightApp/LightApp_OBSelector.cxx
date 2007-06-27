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
#include "LightApp_OBSelector.h"

#include "LightApp_DataOwner.h"
#include "LightApp_DataObject.h"
#include "LightApp_Application.h"

// temporary commented
//#include <OB_Browser.h>

#include <SUIT_Session.h>
#include <SUIT_DataObjectIterator.h>

#include <QTime>

#include <time.h>

/*!
  Constructor
*/
LightApp_OBSelector::LightApp_OBSelector( OB_Browser* ob, SUIT_SelectionMgr* mgr )
  : SUIT_Selector( mgr/* temporary commented : , ob */ )/*,*/
  // temporary commented : myBrowser( ob )
{
  // temporary commented
  /*if ( myBrowser ) {
    connect( myBrowser, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
  }*/    

  setModified();
}

/*!
  Destructor
*/
LightApp_OBSelector::~LightApp_OBSelector()
{
}

/*!
  Gets browser.
*/
OB_Browser* LightApp_OBSelector::browser() const
{
  return myBrowser;
}

/*!
  Gets selection.
*/
void LightApp_OBSelector::getSelection( SUIT_DataOwnerPtrList& theList ) const
{
  if (mySelectedList.count() == 0 ) {
    SUIT_Session* session = SUIT_Session::session();
    SUIT_Application* sapp = session ? session->activeApplication() : 0;
    LightApp_Application* app = dynamic_cast<LightApp_Application*>( sapp );
    if( !app )
      return;

    if ( !myBrowser )
      return;
    DataObjectList objlist;
    // temporary commented
    //myBrowser->getSelected( objlist );
    LightApp_OBSelector* that = (LightApp_OBSelector*)this;
    QListIterator<SUIT_DataObject*> it( objlist );
    while ( it.hasNext() )
    {
      LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( it.next() );
      if ( obj && app->checkDataObject(obj) )
      {
#ifndef DISABLE_SALOMEOBJECT
        Handle(SALOME_InteractiveObject) aSObj = new SALOME_InteractiveObject
          ( obj->entry().toLatin1().constData(),
	    obj->componentDataType().toLatin1().constData(),
	    obj->name().toLatin1().constData() );
        LightApp_DataOwner* owner = new LightApp_DataOwner( aSObj  );
#else
        LightApp_DataOwner* owner = new LightApp_DataOwner( obj->entry() );
#endif
        that->mySelectedList.append( SUIT_DataOwnerPtr( owner ) );
      }
    }
  }
  theList = mySelectedList;
}

/*!Sets selection.*/
void LightApp_OBSelector::setSelection( const SUIT_DataOwnerPtrList& theList )
{
  if ( !myBrowser )
    return;

  if( myEntries.count() == 0 
      // temporary commented
      /*||
	myModifiedTime < myBrowser->getModifiedTime()*/ )
    fillEntries( myEntries );

  DataObjectList objList;
  for ( SUIT_DataOwnerPtrList::const_iterator it = theList.begin(); it != theList.end(); ++it )
  {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*it).operator->() );
    if ( owner && myEntries.contains( owner->entry() ) )
      objList.append( myEntries[owner->entry()] );
  }

  // temporary commented
  //myBrowser->setSelected( objList );
  mySelectedList.clear();
}

/*!On selection changed.*/
void LightApp_OBSelector::onSelectionChanged()
{
  QTime t1 = QTime::currentTime();
  mySelectedList.clear();
  selectionChanged();
  QTime t2 = QTime::currentTime();
  qDebug( QString( "selection time = %1 msecs" ).arg( t1.msecsTo( t2 ) ).toLatin1().constData() );
}

/*!Fill entries.*/
void LightApp_OBSelector::fillEntries( QMap<QString, LightApp_DataObject*>& entires )
{
  entires.clear();

  if ( !myBrowser )
    return;

  // temporary commented
  /*for ( SUIT_DataObjectIterator it( myBrowser->getRootObject(),
                                    SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
  {
    LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( it.current() );
    if ( obj )
      entires.insert( obj->entry(), obj );
  }*/

  setModified();
}

/*!Update modified time.*/
void LightApp_OBSelector::setModified()
{
  myModifiedTime = clock();
}
