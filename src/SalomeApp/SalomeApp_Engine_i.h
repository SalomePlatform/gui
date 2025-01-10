// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
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

//  SalomeApp_Engine_i : implementation of SalomeApp_Engine.idl
//  File   : SalomeApp_Engine_i.hxx
//  Author : Alexander SLADKOV

#ifndef _SALOMEAPP_ENGINE_I_HXX_
#define _SALOMEAPP_ENGINE_I_HXX_

#include "SalomeApp.h"

#include "SALOME_Component_i.hxx"

#include <QString>
#include <vector> 
#include <map> 

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SalomeApp_Engine)

class SALOME_NamingService_Abstract;

class SALOMEAPPIMPL_EXPORT SalomeApp_Engine_i: public POA_SalomeApp::Engine,
					   public Engines_Component_i
{
public:
  SalomeApp_Engine_i( const char* theComponentName );
  ~SalomeApp_Engine_i();

  SALOMEDS::TMPFile*      Save( SALOMEDS::SComponent_ptr theComponent, 
                               const char* theURL, 
                               bool isMultiFile );

  CORBA::Boolean          Load( SALOMEDS::SComponent_ptr theComponent, 
                               const SALOMEDS::TMPFile& theFile, 
                               const char* theURL, 
                               bool isMultiFile );

  virtual Engines::TMPFile* DumpPython(CORBA::Boolean isPublished,
                                       CORBA::Boolean isMultiFile,
                                       CORBA::Boolean& isValidScript);

public:
  typedef std::vector<std::string> ListOfFiles;
  typedef std::map<int, ListOfFiles> MapOfFiles;

  ListOfFiles             GetListOfFiles (int);
  void                    SetListOfFiles (int, const ListOfFiles& theListOfFiles);

  static std::string         EngineIORForComponent( const char* theComponentName,
						    bool toCreate );
  static SalomeApp_Engine_i* GetInstance          ( const char* theComponentName,
						    bool toCreate ); 

  static CORBA::Object_ptr   EngineForComponent( const char* theComponentName,
						 bool toCreate  );

public:
  // methods from SALOMEDS::Driver without implementation.  Must be redefined because 
  // there is no default implementation of SALOMEDS::Driver interface
  SALOMEDS::TMPFile* SaveASCII( SALOMEDS::SComponent_ptr, const char*, bool )                                                                        {return 0;}
  CORBA::Boolean LoadASCII( SALOMEDS::SComponent_ptr, const SALOMEDS::TMPFile&, const char*, bool )                                                  {return 0;}
  void Close( SALOMEDS::SComponent_ptr )                                                                                                             {}
  char* ComponentDataType();
  char* IORToLocalPersistentID( SALOMEDS::SObject_ptr, const char*, CORBA::Boolean,  CORBA::Boolean )                                                {return 0;}
  char* LocalPersistentIDToIOR( SALOMEDS::SObject_ptr, const char*, CORBA::Boolean,  CORBA::Boolean )                                                {return 0;}
  bool CanPublishInStudy( CORBA::Object_ptr )                                                                                                        {return 0;}
  SALOMEDS::SObject_ptr PublishInStudy( SALOMEDS::SObject_ptr, CORBA::Object_ptr, const char* )					                     {return 0;}
  CORBA::Boolean CanCopy( SALOMEDS::SObject_ptr )                                                                                                    {return 0;}
  SALOMEDS::TMPFile* CopyFrom( SALOMEDS::SObject_ptr, CORBA::Long& )                                                                                 {return 0;}
  CORBA::Boolean CanPaste( const char*, CORBA::Long )                                                                                                {return 0;}
  SALOMEDS::SObject_ptr PasteInto( const SALOMEDS::TMPFile&, CORBA::Long, SALOMEDS::SObject_ptr )                                                    {return 0;}

  bool keepFiles() const { return myKeepFiles; }
  void keepFiles( bool keep ) { myKeepFiles = keep; }

  char* getVersion();

private:
  static CORBA::ORB_var              orb();
  static PortableServer::POA_var     poa();
  static SALOME_NamingService_Abstract*       namingService();
private:
  MapOfFiles                         myListOfFiles;
  bool                               myKeepFiles;

  QString                            myComponentName;
};

#endif

