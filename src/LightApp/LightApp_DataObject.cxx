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
// File   : LightApp_DataObject.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
// 

#include "LightApp_DataObject.h"
#include "LightApp_Study.h"

#include <CAM_DataModel.h>
#include <CAM_Module.h>
#include <SUIT_DataObjectKey.h>

/*!
  \class LightApp_DataObject::Key
  \brief Represents unique data object key for the LightApp_DataObject
  class instances.
  \internal
*/

class LightApp_DataObject::Key : public SUIT_DataObjectKey
{
public:
  Key( const QString& );
  virtual ~Key();

  virtual bool isLess( const SUIT_DataObjectKey* ) const;
  virtual bool isEqual( const SUIT_DataObjectKey* ) const;

private:
  QString myEntry;
};

/*!
  \brief Constructor.
  \internal
  \param entry data object entry
*/
LightApp_DataObject::Key::Key( const QString& entry )
: SUIT_DataObjectKey(),
  myEntry( entry )
{
}

/*!
  \brief Destructor.
  \internal
*/
LightApp_DataObject::Key::~Key()
{
}

/*!
  \brief Compares this key with the another one.
  \internal
  \param other another data object key
  \return \c true if this key is less than \a other.
*/
bool LightApp_DataObject::Key::isLess( const SUIT_DataObjectKey* other ) const
{
  Key* that = (Key*)other;
  return myEntry < that->myEntry;
}

/*!
  \brief Compares this key with the another one.
  \internal
  \param other another data object key
  \return \c true if this key is equal to the \a other.
*/
bool LightApp_DataObject::Key::isEqual( const SUIT_DataObjectKey* other ) const
{
  Key* that = (Key*)other;
  return myEntry == that->myEntry;
}

/*!
  \class LightApp_DataObject
  \brief Base data object class to build the data model for all the SALOME modules.
*/

/*!
  \brief Constructor. 
  \param parent parent data object
*/
LightApp_DataObject::LightApp_DataObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ), 
  myCompObject( 0 ), 
  myCompDataType( "" )
{
}

/*!
  \brief Destructor.
*/
LightApp_DataObject::~LightApp_DataObject()
{
}

/*!
  \brief Get the number of the columns provided by the data tree.
  \return number of the columns
*/
int LightApp_DataObject::columnCount() const
{
  // add "Entry" column
  return CAM_DataObject::columnCount() + 1;
}

/*!
  \brief Get column title.
  \param index column index
  \return title of the specified column
*/
QString LightApp_DataObject::columnTitle( const int index ) const
{
  // add "Entry" column
  return index == EntryIdx ? QObject::tr( "ENTRY_COLUMN" ) 
                           : CAM_DataObject::columnTitle( index );
}

/*!
  \brief Check if the column should appear in the tree view header popup menu
  (to show/hide the column).
  \param index column index
  \return \c true if the column can be shown/hidden
*/
bool LightApp_DataObject::appropriate( const int index ) const
{
  // add "Entry" column
  return index == EntryIdx ? true : CAM_DataObject::appropriate( index );
}

/*!
  \brief Get object string identifier.

  This method should be reimplemented in the subclasses.
  Default implementation returns null string.

  \return object ID
*/
QString LightApp_DataObject::entry() const
{
  return QString();
}

/*!
  \brief Get the data object unique key.
  \return data object key
*/
SUIT_DataObjectKey* LightApp_DataObject::key() const
{
  QString str = entry();
  return new Key( str );
}

/*!
  \brief Get object text data for the specified column.

  Column with \a index = 0 (NameIdx) is supposed to be used
  to get the object name.
  Column with \a index = 1 (EntryIdx) is supposed to be used
  to get the object entry.

  \param index column index
  \return object text data
*/
QString LightApp_DataObject::text( const int index ) const
{
  // add "Entry" column
  return index == EntryIdx ? entry() : CAM_DataObject::text( index );
}

/*!
  \brief Get the component object.
  \return component data object
*/
SUIT_DataObject* LightApp_DataObject::componentObject() const
{
  if ( !myCompObject ) {
    SUIT_DataObject* compObj = (SUIT_DataObject*)this;

    while ( compObj && compObj->parent() && compObj->parent() != root() ) {
      compObj = compObj->parent();
    }
    LightApp_DataObject* that = (LightApp_DataObject*)this;
    that->myCompObject = compObj;
  }
  return myCompObject;
}

/*!
  \brief Get component type.
  \return component type
*/
QString LightApp_DataObject::componentDataType() const
{
  if ( myCompDataType.isEmpty() ) {
    SUIT_DataObject* aCompObj = componentObject();
    LightApp_ModuleObject* anObj = dynamic_cast<LightApp_ModuleObject*>( aCompObj );
    if ( anObj ) {
      CAM_DataModel* aModel = anObj->dataModel();
      if ( aModel ) {
        LightApp_DataObject* that = (LightApp_DataObject*)this;
        that->myCompDataType = aModel->module()->name();
      }
    }
  }
  return myCompDataType;
}

/*!
  \class LightApp_ModuleObject
  \brief Used for optimized access to the data model from the data objects.
  \sa CAM_ModuleObject class
*/

/*!
  \brief Constructor.
  \param parent parent data object
*/
LightApp_ModuleObject::LightApp_ModuleObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  LightApp_DataObject( parent ),
  CAM_ModuleObject( parent )
{
}

/*!
  \brief Constructor.
  \param dm data model
  \param parent parent data object
*/
LightApp_ModuleObject::LightApp_ModuleObject( CAM_DataModel* dm, SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  LightApp_DataObject( parent ),
  CAM_ModuleObject( dm, parent )
{
}

/*
  \brief Destructor.
*/
LightApp_ModuleObject::~LightApp_ModuleObject()
{
}

/*!
  \brief Get module name.
  \return module name
*/
QString LightApp_ModuleObject::name() const
{
  return CAM_ModuleObject::name();
}

/*!
  \brief Get data object icon for the specified column.
  \param index column index
  \return object icon for the specified column
*/
QPixmap LightApp_ModuleObject::icon( const int index ) const
{
  return CAM_ModuleObject::icon( index );
}

/*!
  \brief Get data object tooltip for the specified column.
  \param index column index
  \return object tooltip for the specified column
*/
QString LightApp_ModuleObject::toolTip( const int index ) const
{
  return CAM_ModuleObject::toolTip( index );
}

/*!
  \brief Insert new child object to the children list at specified position.
  
  Adds component in the study for this module object if it is not done yet.
  
  \param obj object to be inserted
  \param pos position at which data object should be inserted
*/
void LightApp_ModuleObject::insertChild( SUIT_DataObject* obj, int pos )
{
  LightApp_DataObject::insertChild( obj, pos );

  CAM_DataModel* aModel = dataModel();

  LightApp_RootObject* aRoot = dynamic_cast<LightApp_RootObject*>( parent() );

  if ( aRoot )
    aRoot->study()->addComponent( aModel );
}

/*!
  \class LightApp_RootObject
  \brief Root data object for the light (without CORBA) SALOME application.

  This class is to be instanciated by only one object - the root object
  of the LightApp data object tree. This object is not shown in the object browser.
  The goal of this class is to provide a unified access to LightApp_Study
  object from LightApp_DataObject instances.
*/

/*
  \brief Constructor.
  \param study study
*/
LightApp_RootObject::LightApp_RootObject( LightApp_Study* study )
: CAM_DataObject( 0 ),
  LightApp_DataObject( 0 ), 
  myStudy( study ) 
{
}

/*
  \brief Destructor.
*/
LightApp_RootObject::~LightApp_RootObject()
{
}

/*
  \brief Set study.
  \param study pointer to the study
*/
void LightApp_RootObject::setStudy( LightApp_Study* study )
{ 
  myStudy = study; 
}

/*
  \brief Get study
  \return pointer to the study
*/
LightApp_Study* LightApp_RootObject::study() const
{ 
  return myStudy;  
} 
