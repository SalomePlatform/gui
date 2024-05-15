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

// File:      SalomeApp_Application.h
// Created:   10/22/2004 3:37:25 PM
// Author:    Sergey LITONIN
//
#ifndef SALOMEAPP_APPLICATION_H
#define SALOMEAPP_APPLICATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SalomeApp.h"
#include <LightApp_Application.h>

#include <omniORB4/CORBA.h>

//#include <SALOMEconfig.h>
//#include CORBA_CLIENT_HEADER(SALOMEDS)
#include <SALOME_NamingService.hxx>

#include "SALOMEDSClient.hxx"

#include <QPointer>

#include <memory>

class LightApp_Preferences;
class SalomeApp_Study;
#ifndef DISABLE_PYCONSOLE
class SalomeApp_NoteBook;
#endif
class SUIT_Desktop;
class SALOME_LifeCycleCORBA;


#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class SalomeApp_Application
  \brief Application containing SalomeApp module or LightApp module
*/

class SALOMEAPPIMPL_EXPORT SalomeApp_Application : public LightApp_Application
{
  Q_OBJECT

public:
  enum { MenuToolsId = 5 };
  enum { DumpStudyId = LightApp_Application::UserID, LoadScriptId, PropertiesId,
         CatalogGenId, RegDisplayId, FindActionId, SaveGUIStateId, ConnectId, DisconnectId,
         UserID };

  typedef enum { WT_NoteBook = LightApp_Application::WT_User,
                 WT_User
               } WindowTypes;

protected:
  enum { OpenRefresh = LightApp_Application::OpenReload + 1 };
  enum { CloseDisconnectSave = LightApp_Application::CloseDiscard + 1, CloseDisconnect };
  enum { LoadStudyId = LightApp_Application::OpenStudyId + 1, NewAndScriptId };

public:
  SalomeApp_Application(SALOME_NamingService_Abstract *ns = nullptr);
  virtual ~SalomeApp_Application();

  virtual void                        updateObjectBrowser( const bool = true );

  virtual void                        start();

  virtual void                        contextMenuPopup( const QString&, QMenu*, QString& );

  virtual bool                        checkDataObject(LightApp_DataObject* theObj);

  virtual bool                        checkExistingDoc();

  static CORBA::ORB_var               orb();
  static _PTR(Study)                  getStudy();
  static SALOME_NamingService_Abstract* namingService();
  static SALOME_LifeCycleCORBA*       lcc();

  SUIT_ViewManager*                   newViewManager(const QString&);
  void                                updateSavePointDataObjects( SalomeApp_Study* );

  virtual bool                        isPossibleToClose( bool& );

  virtual bool                        useStudy( const QString& );
  virtual void                        updateDesktopTitle();

#ifndef DISABLE_PYCONSOLE
  virtual void                        setNoteBook(SalomeApp_NoteBook* theNoteBook);
  virtual SalomeApp_NoteBook*         getNoteBook() const;
#endif

  virtual bool                        renameAllowed( const QString& ) const;
  virtual bool                        renameObject( const QString&, const QString& );

public slots:
  virtual void                        onNewDoc();
  virtual void                        onLoadDoc();
  virtual void                        onNewWithScript();
  virtual bool                        onLoadDoc( const QString& );
  virtual void                        onUnloadDoc( bool ask = true);
  virtual void                        onCloseDoc( bool ask = true);
  virtual void                        onOpenDoc();
  virtual bool                        onOpenDoc( const QString& );
  virtual bool                        onReopenDoc();

  virtual void                        onExit();
  virtual void                        onCopy();
  virtual void                        onPaste();
  void                                onSaveGUIState();// called from VISU
#ifndef DISABLE_PYCONSOLE
  void                                onUpdateStudy(); // called from NoteBook
#endif
  bool                                onRestoreStudy( const QString& theDumpScript,
                                                      const QString& theStudyName,
                                                      bool theIsStudySaved );

  virtual void                        onDockWindowVisibilityChanged( bool );

protected slots:
  void                                onStudyCreated( SUIT_Study* );
  void                                onStudyOpened( SUIT_Study* );
  void                                onDesktopMessage( const QString& );

  virtual void                        onModuleActivation( const QString& );

protected:
  virtual void                        createActions();
  virtual SUIT_Study*                 createNewStudy();
  virtual QWidget*                    createWindow( const int );

  virtual void                        updateCommandsStatus();
  virtual void                        onSelectionChanged();

  virtual void                        createPreferences( LightApp_Preferences* );

  virtual bool                        closeAction( const int, bool& );
  virtual int                         closeChoice( const QString& );

  virtual int                         openChoice( const QString& );
  virtual bool                        openAction( const int, const QString& );

  virtual QMap<int, QString>          activateModuleActions() const;
  virtual void                        moduleActionSelected( const int );

  void                                objectBrowserColumnsVisibility();

#ifndef DISABLE_PYCONSOLE
  bool                                updateStudy();
  virtual PyConsole_Interp*           createPyInterp();
#endif

  virtual bool                        canOpenDoc( const QString& );
  virtual void                        afterCloseDoc();

  virtual void                        addCatalogue( const QString&, const QString& );

private slots:
  void                                onDeleteInvalidReferences();
  void                                onDblClick( SUIT_DataObject* );
  void                                onProperties();
  void                                onDumpStudy();
  void                                onLoadScript();

  void                                onDeleteGUIState();
  void                                onRestoreGUIState();

  void                                onCatalogGen();
  void                                onRegDisplay();
  void                                onFindAction();
  void                                onOpenWith();
  void                                onExtAction();

private:
  void                                createExtraActions();
  void                                ensureShaperIsActivated();

private:
#ifndef DISABLE_PYCONSOLE
  QPointer<SalomeApp_NoteBook>        myNoteBook;        // Notebook instance
#endif
  QMap<QString, QAction*>             myExtActions;      // Map <AttributeUserID, QAction>
  bool                                myIsCloseFromExit; // "Close from Exit" flag

  bool                                myToIgnoreMessages;// to ignore messages from SALOMEDS

signals:
  void                                dumpedStudyClosed( const QString& theDumpScript,
                                                         const QString& theStudyName,
                                                         bool theIsStudySaved );
  void                                notebookVarUpdated( QString theVarName );
  void                                objectDoubleClicked( SUIT_DataObject* );
protected:
  static std::unique_ptr<SALOME_NamingService_Abstract> _ns;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
