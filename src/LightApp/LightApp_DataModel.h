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
// File:      LightApp_DataModel.h
// Created:   10/25/2004 10:32:33 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#ifndef LIGHTAPP_DATAMODEL_H
#define LIGHTAPP_DATAMODEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LightApp.h"
#include "CAM_DataModel.h"

class LightApp_Module;
class LightApp_Study;
class LightApp_DataObject;
class SUIT_DataBrowser;

/*!
  Description : Base class of data model
*/
class LIGHTAPP_EXPORT LightApp_DataModel : public CAM_DataModel
{
  Q_OBJECT

public:
                                      LightApp_DataModel ( CAM_Module* theModule );
  virtual                             ~LightApp_DataModel();

  virtual bool                        open( const QString&, CAM_Study*, QStringList );
  virtual bool                        save( QStringList& );
  virtual bool                        saveAs( const QString&, CAM_Study*, QStringList& );
  virtual bool                        close();

  virtual void                        update( LightApp_DataObject* = 0, LightApp_Study* = 0 );

  virtual bool                        isModified() const;
  virtual bool                        isSaved()  const;

  LightApp_Module*                    getModule() const;

  int  groupId() const;
  void registerColumn( SUIT_DataBrowser*, const QString&, const int );
  void unregisterColumn( SUIT_DataBrowser*, const QString& );

signals:
  void                                opened();
  void                                saved();
  void                                closed();

protected:
  LightApp_Study*                     getStudy() const;
  virtual void                        build();
  virtual void                        updateWidgets();

private:
  int myGroupId;
};

#endif 