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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007


#ifndef SALOME_PYQT_DATAOBJECTLIGHT_H
#define SALOME_PYQT_DATAOBJECTLIGHT_H


#include "SALOME_PYQT_GUILight.h"
#include <LightApp_DataObject.h>
#include <LightApp_Study.h>

#include <qstring.h>

/*!
 * SALOME_PYQT_DataObjectLight - PYTHON LIGHT module's data object class
 */
class SALOME_PYQT_LIGHT_EXPORT SALOME_PYQT_DataObjectLight : public virtual LightApp_DataObject
{

 public:
  SALOME_PYQT_DataObjectLight( SUIT_DataObject* = 0 );

  virtual ~SALOME_PYQT_DataObjectLight();
  
  virtual QString    entry() const;
  
  virtual QString    name()    const;
  QPixmap            icon(const int = NameId)    const;
  QString            toolTip(const int = NameId) const;
  
  void setName(const QString& name);
  void setIcon(const QString& icon);
  void setToolTip(const QString& tooltip);

 private:
  QString myEntry;
  QString myName;
  QString myToolTip;
  QPixmap myIcon;
}; 

#endif // SALOME_PYQT_DATAOBJECTLIGHT_H
