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
#include "CAM_RootObject.h"

#include "CAM_DataModel.h"
#include "CAM_Module.h"

/*!
  \class CAM_RootObject
  \brief CAM data model root object.
  
  This class is intended for optimized access to CAM_DataModel instance
  from CAM_DataObject instances.

  To take advantage of this class in a specific application, 
  custom data model root object class should be derived from both CAM_RootObject
  and application-specific DataObject implementation using virtual inheritance.
*/

/*!
  \brief Constructor.
  \param parent parent data object
*/
CAM_RootObject::CAM_RootObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  myDataModel( 0 )
{
}

/*!
  \brief Constructor.
  \param data data model
  \param parent parent data object
*/
CAM_RootObject::CAM_RootObject( CAM_DataModel* data, SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  myDataModel( data )
{
}

/*!
  \brief Destructor.

  Does nothing.
*/
CAM_RootObject::~CAM_RootObject()
{
}

/*!
  \brief Get root object name.

  If the data model is set, this method returns module name.
  Otherwise returns empty string.

  \return root object name
*/
QString CAM_RootObject::name() const
{
  QString aName = "";
  if (myDataModel)
    aName = myDataModel->module()->moduleName();
  return aName;
}

/*!
  \brief Get data model.
  \return data model pointer or 0 if it is not set
*/
CAM_DataModel* CAM_RootObject::dataModel() const
{
  return myDataModel;
}

/*!
  \brief Set data model.
  \param dm data model
*/
void CAM_RootObject::setDataModel( CAM_DataModel* dm )
{
  myDataModel = dm;
}
