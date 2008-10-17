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
// File:      LightApp_Application.h
// Created:   6/20/2005 18:39:25 PM
// Author:    OCC team
// Copyright (C) CEA 2005

#ifndef LIGHTAPP_APPLICATION_H
#define LIGHTAPP_APPLICATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LightApp.h"
#include <CAM_Application.h>

#include <QPointer>

class LogWindow;
#ifndef DISABLE_PYCONSOLE
class PyConsole_Console;
#endif
class LightApp_WidgetContainer;
class LightApp_Preferences;
class LightApp_SelectionMgr;
class LightApp_DataObject;
class SUIT_DataBrowser;
class SUIT_Study;
class SUIT_Accel;
class CAM_Module;

class QString;
class QWidget;
class QStringList;
class QDockWidget;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  Description : Application containing only LightApp module
*/

class LIGHTAPP_EXPORT LightApp_Application : public CAM_Application
{
  Q_OBJECT

public:
  typedef enum { WT_ObjectBrowser,
#ifndef DISABLE_PYCONSOLE
                 WT_PyConsole,
#endif
                 WT_LogWindow,
                 WT_User }
  WindowTypes;

  enum { MenuWindowId = 6 };

  enum { RenameId = CAM_Application::UserID,
	 CloseId, CloseAllId, GroupAllId,
	 PreferencesId, MRUId, ModulesListId,
         NewGLViewId, NewPlot2dId, NewOCCViewId, NewVTKViewId, NewQxGraphViewId, UserID };

protected:
  enum { NewStudyId = 1, OpenStudyId };

public:
  LightApp_Application();
  virtual ~LightApp_Application();

  virtual QString                     applicationName() const;
  virtual QString                     applicationVersion() const;

  virtual CAM_Module*                 loadModule( const QString& );
  virtual bool                        activateModule( const QString& );

  LightApp_SelectionMgr*              selectionMgr() const;

  LogWindow*                          logWindow();
  SUIT_DataBrowser*                   objectBrowser();
#ifndef DISABLE_PYCONSOLE
  PyConsole_Console*                  pythonConsole();
#endif

  virtual void                        updateObjectBrowser( const bool = true );

  LightApp_Preferences*               preferences() const;

  virtual QString                     getFileFilter() const;

  virtual QString                     getFileName( bool open, const QString& initial, const QString& filters,
						   const QString& caption, QWidget* parent );
  virtual QString                     getDirectory( const QString& initial, const QString& caption, QWidget* parent );
  virtual QStringList                 getOpenFileNames( const QString& initial, const QString& filters,
							const QString& caption, QWidget* parent );

  void                                updateActions();

  SUIT_ViewManager*                   getViewManager( const QString&, const bool );
  virtual void                        addViewManager( SUIT_ViewManager* );
  virtual void                        removeViewManager( SUIT_ViewManager* );
  virtual SUIT_ViewManager*           createViewManager( const QString& vmType );

  QWidget*                            getWindow( const int, const int = -1 );
  QWidget*                            dockWindow( const int ) const;
  void                                removeDockWindow( const int );
  void                                insertDockWindow( const int, QWidget* );
  void                                placeDockWindow( const int, Qt::DockWidgetArea );

  virtual void                        start();

  virtual void                        contextMenuPopup( const QString&, QMenu*, QString& );

  virtual void                        createEmptyStudy();

  SUIT_Accel*                         accel() const;

  void                                setDefaultStudyName( const QString& theName );

  static int                          studyId();

  virtual bool                        event( QEvent* );

  virtual bool                        checkDataObject( LightApp_DataObject* theObj );

  virtual void                        updateDesktopTitle();

signals:
  void                                studyOpened();
  void                                studySaved();
  void                                studyClosed();
  void                                preferenceChanged( const QString&, const QString&, const QString& );

public slots:
  virtual void                        onHelpContentsModule();
  virtual void                        onHelpContextModule( const QString&, const QString&, const QString& = QString() );
  virtual void                        onNewDoc();
  virtual void                        onOpenDoc();
  virtual void                        onHelpAbout();
  virtual bool                        onOpenDoc( const QString& );

protected:
  virtual void                        createActions();
  virtual void                        createActionForViewer( const int id,
                                                             const int parentId,
                                                             const QString& suffix,
                                                             const int accel );
  virtual SUIT_Study*                 createNewStudy();
  virtual QWidget*                    createWindow( const int );
  virtual void                        defaultWindows( QMap<int, int>& ) const;
  void                                defaultViewManagers( QStringList& ) const;

  virtual void                        setActiveStudy( SUIT_Study* );
  virtual void                        updateCommandsStatus();
  virtual void                        onSelectionChanged();

  virtual void                        beforeCloseDoc( SUIT_Study* );
  virtual void                        afterCloseDoc();
  virtual void                        moduleAdded( CAM_Module* );
  virtual bool                        activateModule( CAM_Module* = 0 );

  LightApp_Preferences*               preferences( const bool ) const;
  virtual void                        createPreferences( LightApp_Preferences* );
  virtual void                        preferencesChanged( const QString&, const QString& );

  virtual void                        loadPreferences();
  virtual void                        savePreferences();

  virtual QMap<int, QString>          activateModuleActions() const;
  virtual void                        moduleActionSelected( const int );

protected slots:
  virtual void                        onDesktopActivated();

  void                                onNewWindow();
  void                                onModuleActivation( const QString& );
  void                                onCloseView( SUIT_ViewManager* );

  void                                onStudyCreated( SUIT_Study* );
  void                                onStudyOpened( SUIT_Study* );
  void                                onStudySaved( SUIT_Study* );
  void                                onStudyClosed( SUIT_Study* );

  void                                onWCDestroyed( QObject* );

  void                                onMRUActivated( const QString& );

private slots:
  void                                onSelection();
  void                                onRefresh();
  void                                onPreferences();
  void                                onPreferenceChanged( QString&, QString&, QString& );
  void                                onRenameWindow();
  void                                onCloseWindow();
  void                                onCloseAllWindow();
  void                                onGroupAllWindow();

protected:
  void                                updateWindows();
  void                                updateViewManagers();
  void                                updateModuleActions();

  void                                loadDockWindowsState();
  void                                saveDockWindowsState();

  virtual void                        studyOpened( SUIT_Study* );
  virtual void                        studyCreated( SUIT_Study* );

  void                                updatePreference( const QString&, const QString&, const QString& );

  QString                             defaultModule() const;
  void                                currentWindows( QMap<int, int>& ) const;
  void                                currentViewManagers( QStringList& ) const;
  void                                moduleIconNames( QMap<QString, QString>& ) const;

  bool                                isLibExists( const QString& ) const;

  QDockWidget*                        windowDock( QWidget* ) const;
  QByteArray                          dockWindowsState( const QMap<QString, bool>&, const QMap<QString, bool>& ) const;
  void                                dockWindowsState( const QByteArray&, QMap<QString, bool>&, QMap<QString, bool>& ) const;

  virtual int                         openChoice( const QString& );
  virtual bool                        openAction( const int, const QString& );

  void                                showPreferences( const QString& = QString() );

protected:
  typedef QPointer<QWidget>         WinPtr;
  typedef QMap<int, WinPtr>         WinMap;
  typedef QMap<QString, QByteArray> WinVis;
  typedef QMap<QString, QByteArray> WinGeom;

  enum { OpenReload = CAM_Application::OpenExist + 1 };

protected:
  LightApp_Preferences*               myPrefs;
  LightApp_SelectionMgr*              mySelMgr;

  WinMap                              myWin;
  WinVis                              myWinVis;
  WinGeom                             myWinGeom;

  SUIT_Accel*                         myAccel;

  static LightApp_Preferences*        _prefs_;

  static int                          lastStudyId;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
