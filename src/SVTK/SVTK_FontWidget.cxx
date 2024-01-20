// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

//  VISU VISUGUI : GUI for SMESH component
//  File   : 
//  Author : Sergey LITONIN
//  Module : 
//
#include "SVTK_FontWidget.h"

#include <QComboBox>

#include <vtkTextProperty.h>

/*!
 * Class       : SVTK_FontWidget
 * Description : Dialog for specifynig font
 */

/*!
  Constructor
*/
SVTK_FontWidget::SVTK_FontWidget( QWidget* theParent )
: ViewerTools_FontWidgetBase( theParent )
{
}

/*!
  Destructor
*/
SVTK_FontWidget::~SVTK_FontWidget()
{
}

void SVTK_FontWidget::InitializeFamilies()
{
  myFamily->insertItem( myFamily->count(), tr( "ARIAL" ) );
  myFamily->insertItem( myFamily->count(), tr( "COURIER" ) );
  myFamily->insertItem( myFamily->count(), tr( "TIMES" ) );
}

void SVTK_FontWidget::SetData( const QColor& theColor,
                               const int theFamily,
                               const bool theBold,
                               const bool theItalic,
                               const bool theShadow )
{
  ViewerTools_FontWidgetBase::SetData( theColor,
                                       theFamily,
                                       theBold,
                                       theItalic,
                                       theShadow );

  if ( theFamily == VTK_ARIAL )
    myFamily->setCurrentIndex( 0 );
  else if ( theFamily == VTK_COURIER )
    myFamily->setCurrentIndex( 1 );
  else
    myFamily->setCurrentIndex( 2 );
}

void SVTK_FontWidget::GetData( QColor& theColor,
                               int& theFamily,
                               bool& theBold,
                               bool& theItalic,
                               bool& theShadow ) const
{
  ViewerTools_FontWidgetBase::GetData( theColor,
                                       theFamily,
                                       theBold,
                                       theItalic,
                                       theShadow );

  int anItem = myFamily->currentIndex();
  if ( anItem == 0 )
    theFamily = VTK_ARIAL;
  else if ( anItem == 1 )
    theFamily = VTK_COURIER;
  else
    theFamily = VTK_TIMES;
}
