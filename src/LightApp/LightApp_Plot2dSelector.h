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
#ifndef LIGHTAPP_PLOT2DSELECTOR_H
#define LIGHTAPP_PLOT2DSELECTOR_H

#include "LightApp.h"

#include <SUIT_Selector.h>
#include <SUIT_DataOwner.h>
#include <SPlot2d_ViewModel.h>

class LightApp_DataObject;

/*!
  \class LightApp_Plot2dSelector
  Custom selector to get/set selection from object browser
*/
class LIGHTAPP_EXPORT LightApp_Plot2dSelector : public QObject, public SUIT_Selector
{
  Q_OBJECT

public:
  LightApp_Plot2dSelector( SPlot2d_Viewer*, SUIT_SelectionMgr* );
  virtual ~LightApp_Plot2dSelector();

  /*!Return "ObjectBrowser"*/
  virtual QString type() const { return "PLot2dViewer"; }

private slots:
  void         onSelectionChanged( const QString& );

protected:
  virtual void getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void setSelection( const SUIT_DataOwnerPtrList& );

private:
  QString myCurEntry;
};

#endif