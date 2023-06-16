// Copyright (C) 2007-2023  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

//  SALOME TOOLSGUI : implementation of desktop "Tools" optioins
//  File   : ToolsGUI.cxx
//  Author : Nicolas REJNERI
//
#include "ToolsGUI.h"
#include <SALOMEDSClient_ClientFactory.hxx>
#include <SALOME_KernelServices.hxx>

/*!
  \class ToolsGUI
  \brief Utility class.
*/

/*!
  \brief Get visibility value of the "AttributeGraphic" attribute.
  \param theObj object
  \param theId sub-object identifier
  \return \c true if an object (sub-object) is visible
*/
bool ToolsGUI::GetVisibility( _PTR(SObject) theObj,
                              void*         theId )
{
  // todo: this function seems to be not used
  _PTR(GenericAttribute) anAttr;
  if ( theObj && theObj->FindAttribute( anAttr, "AttributeGraphic" ) )
  {
    _PTR(AttributeGraphic) aGraphic (anAttr);
    return aGraphic->GetVisibility( int(reinterpret_cast<long long>(theId)) ); // todo: unsafe - converting pointer to int can give non-unique result
  }

  return false;
}

/*!
  \brief Set visibility value of the "AttributeGraphic" attribute.
  \param theObj object
  \return theValue new visibility value
  \param theId sub-object identifier
*/
bool ToolsGUI::SetVisibility( const char* theEntry,
                              const bool  theValue,
                              void*       theId )
{
  // todo: this function seems to be not used
  _PTR(Study) aStudy = ClientFactory::Study(KERNEL::getStudyServant());
  _PTR(SObject) anObj ( aStudy->FindObjectID( theEntry ) );

  if ( anObj )
  {
    _PTR(GenericAttribute) aGAttr;
    if ( anObj->FindAttribute( aGAttr, "AttributeGraphic" ) )
    {
      _PTR(AttributeGraphic) anAttr ( aGAttr );
      anAttr->SetVisibility( int(reinterpret_cast<long long>(theId)), theValue ); // todo: unsafe - converting pointer to int can give non-unique result
    }
    else if ( theValue )
    {
      _PTR(StudyBuilder) aBuilder (aStudy->NewBuilder());
      _PTR(AttributeGraphic) anAttr (aBuilder->FindOrCreateAttribute(anObj, "AttributeGraphic"));
      anAttr->SetVisibility( int(reinterpret_cast<long long>(theId)), theValue ); // todo: unsafe - converting pointer to int can give non-unique result
    }
    return true;
  }

  return false;
}

