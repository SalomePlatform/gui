//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "VTKViewer_MarkerDlg.h"
#include "VTKViewer_MarkerWidget.h"

#include <QFrame>
#include <QHBoxLayout>

/*!
 * Class       : VTKViewer_MarkerDlg
 * Description : Dialog for specifying point marker parameters
 */

/*!
  Constructor
*/
VTKViewer_MarkerDlg::VTKViewer_MarkerDlg( QWidget* theParent )
: QtxDialog( theParent, true, true )
{
  setWindowTitle( tr( "SET_MARKER_TLT" ) );

  myMarkerWidget = new VTKViewer_MarkerWidget( mainFrame() );

  QBoxLayout* aTopLayout = new QHBoxLayout( mainFrame() );
  aTopLayout->setSpacing( 0 );
  aTopLayout->setMargin( 0 );
  aTopLayout->addWidget( myMarkerWidget );
}

/*!
  Destructor
*/
VTKViewer_MarkerDlg::~VTKViewer_MarkerDlg()
{
}

void VTKViewer_MarkerDlg::setCustomMarkerMap( VTK::MarkerMap theMarkerMap )
{
  myMarkerWidget->setCustomMarkerMap( theMarkerMap );
}

VTK::MarkerMap VTKViewer_MarkerDlg::getCustomMarkerMap()
{
  return myMarkerWidget->getCustomMarkerMap();
}

void VTKViewer_MarkerDlg::setStandardMarker( VTK::MarkerType theMarkerType, VTK::MarkerScale theMarkerScale )
{
  myMarkerWidget->setStandardMarker( theMarkerType, theMarkerScale );
}

void VTKViewer_MarkerDlg::setCustomMarker( int theId )
{
  myMarkerWidget->setCustomMarker( theId );
}

VTK::MarkerType VTKViewer_MarkerDlg::getMarkerType() const
{
  return myMarkerWidget->getMarkerType();
}

VTK::MarkerScale VTKViewer_MarkerDlg::getStandardMarkerScale() const
{
  return myMarkerWidget->getStandardMarkerScale();
}

int VTKViewer_MarkerDlg::getCustomMarkerID() const
{
  return myMarkerWidget->getCustomMarkerID();
}
