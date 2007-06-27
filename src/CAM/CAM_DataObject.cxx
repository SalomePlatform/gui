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
#include "CAM_DataObject.h"

#include "CAM_Module.h"
#include "CAM_DataModel.h"

/*!
  \class CAM_DataObject
  \brief CAM-based implementation of data object.
  
  In addition to base implementation provides integration
  with CAM_DataModel.
*/

/*!
  \brief Constructor.
  \param parent parent data object
*/
CAM_DataObject::CAM_DataObject( SUIT_DataObject* parent )
: SUIT_DataObject( parent )
{
}

/*!
  \brief Destructor.

  Does nothing.
*/
CAM_DataObject::~CAM_DataObject()
{
}

/*!
  \brief Get CAM module.
  \return parent module object pointer
*/
CAM_Module* CAM_DataObject::module() const
{ 
  CAM_Module* mod = 0;

  CAM_DataModel* data = dataModel();
  if ( data )
    mod = data->module();

  return mod;
}

/*!
  \brief Get CAM data model.
  \return data model or 0 if it is not set
  \sa CAM_RootObject class
*/
CAM_DataModel* CAM_DataObject::dataModel() const
{
  CAM_DataObject* parentObj = dynamic_cast<CAM_DataObject*>( parent() );

  if ( !parentObj )
    return 0;

  return parentObj->dataModel();
}
