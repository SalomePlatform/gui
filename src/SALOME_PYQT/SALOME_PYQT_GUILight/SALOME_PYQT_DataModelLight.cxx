//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007
//
#include "PyInterp_Interp.h" // // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include "SALOME_PYQT_DataModelLight.h" 
#include "SALOME_PYQT_DataObjectLight.h"
#include "SALOME_PYQT_ModuleLight.h"
#include <utilities.h>

#include <LightApp_Study.h>
#include <CAM_Module.h>
#include <CAM_Application.h>
#include <CAM_Study.h>


//=================================================================================
// function : SALOME_PYQT_DataModelLight()
// purpose  : constructor
//=================================================================================
SALOME_PYQT_DataModelLight::SALOME_PYQT_DataModelLight(CAM_Module * theModule)
  : LightApp_DataModel( theModule ),
    myFileName( "" ),
    myStudyURL( "" )
{
  
}

//=================================================================================
// function : ~SALOME_PYQT_DataModelLight()
// purpose  : destructor
//=================================================================================
SALOME_PYQT_DataModelLight::~SALOME_PYQT_DataModelLight()
{
}

//=================================================================================
// function : open()
// purpose  : Open data model operation
//=================================================================================
bool SALOME_PYQT_DataModelLight::open( const QString& theURL, CAM_Study* study, QStringList theListOfFiles)
{
  MESSAGE("SALOME_PYQT_DataModelLight::open()");
  LightApp_Study* aDoc = dynamic_cast<LightApp_Study*>( study );
  SALOME_PYQT_ModuleLight* aModule = dynamic_cast<SALOME_PYQT_ModuleLight*>(module());
  if ( !aDoc || !aModule)
    return false;
  
  LightApp_DataModel::open( theURL, aDoc, theListOfFiles );
  
  return aModule->open(theListOfFiles);
  
}

//=================================================================================
// function : save()
// purpose  : Save data model operation
//=================================================================================
bool SALOME_PYQT_DataModelLight::save( QStringList& theListOfFiles)
{
  MESSAGE("SALOME_PYQT_DataModelLight::save()");
  bool isMultiFile = false; // temporary solution
  
  LightApp_DataModel::save(theListOfFiles);
  LightApp_Study* study = dynamic_cast<LightApp_Study*>( module()->application()->activeStudy() );
  SALOME_PYQT_ModuleLight* aModule = dynamic_cast<SALOME_PYQT_ModuleLight*>(module());

  if(!aModule || !study)
    return false;
  

  std::string aTmpDir = study->GetTmpDir(myStudyURL.toLatin1().constData(), isMultiFile );

  theListOfFiles.append(QString(aTmpDir.c_str()));
  int listSize = theListOfFiles.size();
  aModule->save(theListOfFiles);
  //Return true if in the List of files was added item(s)
  //else return false 
  return theListOfFiles.size() > listSize;
}

//=================================================================================
// function : saveAs()
// purpose  : SaveAs data model operation
//=================================================================================
bool SALOME_PYQT_DataModelLight::saveAs ( const QString& theURL, CAM_Study* theStudy, QStringList& theListOfFiles)
{
  myStudyURL = theURL;
  return save(theListOfFiles);
}



bool SALOME_PYQT_DataModelLight::create( CAM_Study* study )
{
  return true;
}

//=================================================================================
// function : isModified()
// purpose  : default implementation, always returns false so as not to mask study's isModified()
//=================================================================================
bool SALOME_PYQT_DataModelLight::isModified() const
{
  return false;
}

//=================================================================================
// function : isSaved()
// purpose  : default implementation, always returns true so as not to mask study's isSaved()
//=================================================================================
bool SALOME_PYQT_DataModelLight::isSaved() const
{
  return true;
}


//=================================================================================
// function : close()
// purpose  : Close data model operation
//=================================================================================
bool SALOME_PYQT_DataModelLight::close()
{
  LightApp_DataModel::close();
  return true;
}


void SALOME_PYQT_DataModelLight::update ( LightApp_DataObject* theObj, LightApp_Study* theStudy )
{
  // Nothing to do here: we always keep the data tree in the up-to-date state
  // The only goal of this method is to hide default behavior from LightApp_DataModel
  return;
}

LightApp_ModuleObject* SALOME_PYQT_DataModelLight::getRoot()
{
  LightApp_Study* study = dynamic_cast<LightApp_Study*>( module()->application()->activeStudy() );
  LightApp_ModuleObject *aModelRoot = dynamic_cast<LightApp_ModuleObject*>(root());
  if(aModelRoot == NULL) {
    aModelRoot = new LightApp_ModuleObject(this,study->root());
    aModelRoot->setDataModel( this );
    setRoot(aModelRoot);
  }
  return aModelRoot;
}
