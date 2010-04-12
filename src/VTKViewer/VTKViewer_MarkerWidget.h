//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#ifndef VTKVIEWER_MARKERWIDGET_H
#define VTKVIEWER_MARKERWIDGET_H

#include "VTKViewer.h"
#include "VTKViewer_MarkerDef.h"

#include <QWidget>

class QButtonGroup;
class QStackedWidget;

class QtxComboBox;

/*!
 * Class       : VTKViewer_MarkerWidget
 * Description : Widget for specifying point marker parameters
 */
class VTKVIEWER_EXPORT VTKViewer_MarkerWidget : public QWidget
{
  Q_OBJECT

public:
  VTKViewer_MarkerWidget( QWidget* );
  virtual ~VTKViewer_MarkerWidget();

  void             setCustomMarkerMap( VTK::MarkerMap );
  VTK::MarkerMap   getCustomMarkerMap();

  void             setStandardMarker( VTK::MarkerType, VTK::MarkerScale );
  void             setCustomMarker( int );
  VTK::MarkerType  getMarkerType() const;
  VTK::MarkerScale getStandardMarkerScale() const;
  int              getCustomMarkerID() const;

private:
  void             init();
  void             addTexture( int, bool = false );
  QPixmap          markerFromData( const VTK::MarkerData& );

private slots:
  void             browse();

private:
  QButtonGroup*    myTypeGroup;
  QStackedWidget*  myWGStack;
  QtxComboBox*     myStdTypeCombo;
  QtxComboBox*     myStdScaleCombo;
  QtxComboBox*     myCustomTypeCombo;

  VTK::MarkerMap   myCustomMarkerMap;
};

#endif