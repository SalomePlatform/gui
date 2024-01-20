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

// File:      LightApp_Application.cxx
// Created:   6/20/2005 18:39:45 PM
// Author:    Natalia Donis
#ifdef WIN32
// E.A. : On windows with python 2.6, there is a conflict
// E.A. : between pymath.h and Standard_math.h which define
// E.A. : some same symbols : acosh, asinh, ...
  #include <Standard_math.hxx>
  #ifndef DISABLE_PYCONSOLE
    #include <pymath.h>
  #endif
#endif

#ifndef DISABLE_PYCONSOLE
  #include "PyConsole_Interp.h"
  #include "LightApp_PyEditor.h"
  #include "PyConsole_Console.h"
#endif

#include "LightApp_Application.h"
#include "LightApp_Module.h"
#include "LightApp_DataModel.h"
#include "LightApp_DataOwner.h"
#include "LightApp_Displayer.h"
#include "LightApp_Study.h"
#include "LightApp_Preferences.h"
#include "LightApp_PreferencesDlg.h"
#include "LightApp_ModuleDlg.h"
#include "LightApp_AboutDlg.h"
#include "LightApp_ExtInfoDlg.h"
#include "LightApp_ModuleAction.h"
// temporary commented
#include "LightApp_EventFilter.h"
#include "LightApp_OBSelector.h"
#include "LightApp_SelectionMgr.h"
#include "LightApp_DataObject.h"
#include "LightApp_WgViewModel.h"
#include "LightApp_FullScreenHelper.h"

#include <GUI_version.h>

#include <SALOME_Event.h>

#ifdef USE_SALOME_STYLE
#include <Style_Salome.h>
#include <Style_PrefDlg.h>
#endif // USE_SALOME_STYLE

#include <CAM_Module.h>
#include <CAM_DataModel.h>
#include <CAM_Study.h>
#include <STD_TabDesktop.h>

#include <SUIT_DataBrowser.h>
#include <SUIT_Session.h>
#include <SUIT_Study.h>
#include <SUIT_FileDlg.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ShortcutMgr.h>
#include <SUIT_Tools.h>
#include <SUIT_Accel.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ViewWindow.h>

#include <Qtx.h>
#include <QtxFontEdit.h>
#include <QtxToolBar.h>
#include <QtxTreeView.h>
#include <QtxInfoPanel.h>
#include <QtxMRUAction.h>
#include <QtxDockAction.h>
#include <QtxDockWidget.h>
#include <QtxActionToolMgr.h>
#include <QtxSearchTool.h>
#include <QtxWorkstack.h>
#include <QtxMap.h>

#include <LogWindow.h>
#include <SalomeApprc_utils.h>

#ifndef DISABLE_GLVIEWER
  #include <GLViewer_Viewer.h>
  #include <GLViewer_ViewManager.h>
  #include "LightApp_GLSelector.h"
#endif

#ifndef DISABLE_PLOT2DVIEWER
  #include <Plot2d_ViewManager.h>
  #include <Plot2d_ViewModel.h>
  #include <Plot2d_ViewWindow.h>
  #include <Plot2d_ViewFrame.h>
  #include "LightApp_Plot2dSelector.h"
#ifndef DISABLE_SALOMEOBJECT
  #include <SPlot2d_ViewModel.h>
#else
  #include <Plot2d_ViewModel.h>
#endif
#endif

#ifndef DISABLE_OCCVIEWER
  #include <OCCViewer_ViewManager.h>
  #include <OCCViewer_ViewFrame.h>
  #include <OCCViewer_ViewPort3d.h>
#ifndef DISABLE_SALOMEOBJECT
  #include <SOCC_ViewModel.h>
#else
  #include <OCCViewer_ViewModel.h>
#endif
  #include "LightApp_OCCSelector.h"
#endif

#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
  #include <SVTK_ViewModel.h>
  #include <SVTK_ViewManager.h>
  #include "LightApp_VTKSelector.h"
#else
  #include <VTKViewer_ViewModel.h>
  #include <VTKViewer_ViewManager.h>
#endif
  #include <VTKViewer_ViewModel.h>
#endif

#ifndef DISABLE_QXGRAPHVIEWER
  #include <QxScene_ViewManager.h>
  #include <QxScene_ViewModel.h>
  #include <QxScene_ViewWindow.h>
#endif

#ifndef DISABLE_GRAPHICSVIEW
  #include "GraphicsView_Viewer.h"
  #include "GraphicsView_ViewManager.h"
  #include "LightApp_GVSelector.h"
#endif

#ifndef DISABLE_PVVIEWER
  #include "PVViewer_ViewManager.h"
  #include "PVViewer_ViewWindow.h"
  #include "PVViewer_ViewModel.h"
#endif

#ifndef DISABLE_PYVIEWER
  #include <PyViewer_ViewManager.h>
  #include <PyViewer_ViewModel.h>
  #include <PyViewer_ViewWindow.h>
#endif

#ifndef DISABLE_PV3DVIEWER
#ifndef DISABLE_SALOMEOBJECT
  #include <SPV3D_ViewModel.h>
  #include <SPV3D_ViewManager.h>
  #include "LightApp_PV3DSelector.h"
#else
  #include <PV3DViewer_ViewModel.h>
  #include <PV3DViewer_ViewManager.h>
#endif
  #include <PV3DViewer_ViewManager.h>
  #include <PV3DViewer_ViewModel.h>
  #include "PV3DViewer_ViewWindow.h"
#endif


#define VISIBILITY_COLUMN_WIDTH 25

#include <QDir>
#include <QImage>
#include <QString>
#include <QWidget>
#include <QStringList>
#include <QFile>
#include <QApplication>
#include <QMap>
#include <QStatusBar>
#include <QThread>
#include <QObjectList>
#include <QComboBox>
#include <QInputDialog>
#include <QFontDatabase>
#include <QIcon>
#include <QByteArray>
#include <QMenu>
#include <QProcess>
#include <QTimer>
#include <QHeaderView>
#include <QTreeView>
#include <QMimeData>
#include <QShortcut>
#include <QRegExp>

#include <utilities.h>

#define FIRST_HELP_ID 1000000
#define HAS_WWW_URL true
#define HAS_FORUM_URL true
#define HAS_YOUTUBE_URL true
#define HAS_TUTORIAL_URL false

#ifndef DISABLE_SALOMEOBJECT
  #include <SALOME_InteractiveObject.hxx>
  #include <SALOME_ListIO.hxx>
#endif

#include <Standard_Version.hxx>

#define ToolBarMarker    0
#define DockWidgetMarker 1

static const char* imageEmptyIcon[] = {
"20 20 1 1",
".      c None",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"...................."};

// Markers used to parse array with dockable windows and toolbars state.
// For more details please see the qdockarealayout.cpp && qtoolbararealayout.cpp
// in the Qt source code.

#define QDockWidgetMarker 0xfd // = DockWidgetStateMarker
#define QToolBarMarker 0xfc    // = ToolBarStateMarkerEx

// Format of the Byte array with the windows and toolbar state is:
// VersionMarker|version|DockWidgetStateMarker|nbDockWidgetLines|...DocWidgetData...|ToolBarStateMarkerEx|nbToolBarLines|...ToolBarData...

//Find toolbar marker position in the array in the following way:
//since the 'toolbar marker' is not unique, find index of first occurrence of the
//'toolbar marker' in the array and check that next string is name of the toolbar

namespace
{
  const char* salomeAppDir = "SALOME_APPLICATION_DIR";
}

void LightAppCleanUpAppResources()
{
  if ( LightApp_Application::_prefs_ ) {
    delete LightApp_Application::_prefs_;
    LightApp_Application::_prefs_ = 0;
  }
}

namespace
{
  int getToolbarMarkerIndex( QByteArray input, const QStringList& aFlags ) {
    int aResult = -1,tmp = 0;
    int inputLen = input.length();
    QDataStream anInputData( &input, QIODevice::ReadOnly );
    while ( tmp < inputLen ) {
      tmp = input.indexOf( (uchar)QToolBarMarker, tmp + 1 );
      if ( tmp < 0 )
        break;
      anInputData.device()->seek( tmp );
      uchar mark;
      anInputData >> mark;
      int lines;
      anInputData >> lines;

      if ( lines == 0 && anInputData.atEnd() ) {
        //Case then array doesn't contain information about toolbars,
        aResult = tmp;
        break;
      }

      int pos;
      anInputData >> pos;
      int cnt;
      anInputData >> cnt;
      QString str;
      anInputData >> str;
      if ( aFlags.contains( str ) ) {
        aResult = tmp;
        break;
      }
    }
    return aResult;
  }

  QString langToName( const QString& lang )
  {
    // special processing for English language to avoid such result as "American English"
    // as Qt cannot just say "English"
    QString result;
    if ( lang == "en" )
      result = "English";
    else
      result = QLocale( lang ).nativeLanguageName();
    return result;
  }

  QString getHelpItem( const QString& section, const QString& parameter, const QString& root = QString() )
  {
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    foreach( QString item, resMgr->stringValue( section, parameter ).split( ";;", QString::SkipEmptyParts ) )
    {
      if ( item.startsWith( "http", Qt::CaseInsensitive ) )
        return item;
      QString path = item;
      path.remove( QRegExp( "#.*$" ) );
      QFileInfo fi( path );
      if ( fi.isRelative() && !root.isEmpty() )
        path = Qtx::addSlash( root ) + path;
      if ( QFile::exists( path ) )
        return item;
    }
    return QString();
  }

  const bool HAS_SALOME_ON_DEMAND =
#if defined(WITH_SALOME_ON_DEMAND)
    true;
#else
    false;
#endif
}

/*!Create new instance of LightApp_Application.*/
extern "C" LIGHTAPP_EXPORT SUIT_Application* createApplication()
{
  return new LightApp_Application();
}

/*! \var global preferences of LightApp */
LightApp_Preferences* LightApp_Application::_prefs_ = 0;


/*!
  \class LightApp_Application
  Application containing LightApp module
*/

/*!Constructor.*/
LightApp_Application::LightApp_Application()
: CAM_Application( false ),
  myPrefs( 0 ),
  myScreenHelper(new LightApp_FullScreenHelper())
{
  Q_INIT_RESOURCE( LightApp );

  STD_TabDesktop* desk = new STD_TabDesktop();
  desk->setFullScreenAllowed(false);
  desk->setMinimizeAllowed(false);

  setDesktop( desk );

  // initialize auto save timer
  myAutoSaveTimer = new QTimer( this );
  myAutoSaveTimer->setSingleShot( true );
  connect( myAutoSaveTimer, SIGNAL( timeout() ), this, SLOT( onSaveDoc() ) );

  //connect( this, SIGNAL( moving() ), this, SLOT( onMoved() ) );

  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap aLogo = aResMgr->loadPixmap( "LightApp", tr( "APP_DEFAULT_ICO" ), false );

  desktop()->setWindowIcon( aLogo );
  desktop()->setDockableMenuBar( false );
  desktop()->setDockableStatusBar( false );

  // base logo (salome itself)
  desktop()->logoInsert( "_app_base",  aResMgr->loadPixmap( "LightApp", tr( "APP_BASE_LOGO" ), false ) );
  // extra logo (salome-based application)
  desktop()->logoInsert( "_app_extra", aResMgr->loadPixmap( "LightApp", tr( "APP_EXTRA_LOGO" ), false ) );

  clearViewManagers();

  mySelMgr = new LightApp_SelectionMgr( this );

  myAccel = SUIT_Accel::getAccel();

#ifndef DISABLE_OCCVIEWER
  myAccel->setActionKey( SUIT_Accel::PanLeft,     Qt::CTRL+Qt::Key_Left,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanRight,    Qt::CTRL+Qt::Key_Right,    OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanUp,       Qt::CTRL+Qt::Key_Up,       OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanDown,     Qt::CTRL+Qt::Key_Down,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomIn,      Qt::CTRL+Qt::Key_Plus,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomOut,     Qt::CTRL+Qt::Key_Minus,    OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     Qt::CTRL+Qt::Key_Asterisk, OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     Qt::Key_Space,             OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateLeft,  Qt::ALT+Qt::Key_Left,      OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateRight, Qt::ALT+Qt::Key_Right,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateUp,    Qt::ALT+Qt::Key_Up,        OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateDown,  Qt::ALT+Qt::Key_Down,      OCCViewer_Viewer::Type() );
#endif
#ifndef DISABLE_VTKVIEWER
  myAccel->setActionKey( SUIT_Accel::PanLeft,     Qt::CTRL+Qt::Key_Left,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanRight,    Qt::CTRL+Qt::Key_Right,    VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanUp,       Qt::CTRL+Qt::Key_Up,       VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanDown,     Qt::CTRL+Qt::Key_Down,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomIn,      Qt::CTRL+Qt::Key_Plus,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomOut,     Qt::CTRL+Qt::Key_Minus,    VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     Qt::CTRL+Qt::Key_Asterisk, VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     Qt::Key_Space,             VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateLeft,  Qt::ALT+Qt::Key_Left,      VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateRight, Qt::ALT+Qt::Key_Right,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateUp,    Qt::ALT+Qt::Key_Up,        VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateDown,  Qt::ALT+Qt::Key_Down,      VTKViewer_Viewer::Type() );
#endif
#ifndef DISABLE_PLOT2DVIEWER
  myAccel->setActionKey( SUIT_Accel::PanLeft,     Qt::CTRL+Qt::Key_Left,     Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanRight,    Qt::CTRL+Qt::Key_Right,    Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanUp,       Qt::CTRL+Qt::Key_Up,       Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanDown,     Qt::CTRL+Qt::Key_Down,     Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomIn,      Qt::CTRL+Qt::Key_Plus,     Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomOut,     Qt::CTRL+Qt::Key_Minus,    Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     Qt::CTRL+Qt::Key_Asterisk, Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     Qt::Key_Space,             Plot2d_Viewer::Type() );
#endif

  connect( mySelMgr, SIGNAL( selectionChanged() ), this, SLOT( onSelection() ) );
  connect( desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
           this,      SLOT( onWindowActivated( SUIT_ViewWindow* ) ), Qt::UniqueConnection );
  connect( this, SIGNAL( viewManagerRemoved( SUIT_ViewManager* ) ),
           this, SLOT( onViewManagerRemoved( SUIT_ViewManager* ) ), Qt::UniqueConnection );


  // Set existing font for the python console in resources
  if( !aResMgr->hasValue( "PyConsole", "font" ) )
    return;

  QFont f = aResMgr->fontValue( "PyConsole", "font" );
  QFontDatabase fdb;
  QStringList famdb = fdb.families();

  if ( famdb.contains(f.family()) || !aResMgr->hasValue( "PyConsole", "additional_families" ) )
    return;

  QStringList anAddFamilies = aResMgr->stringValue( "PyConsole", "additional_families" ).split( ";", QString::SkipEmptyParts );
  QString aFamily;
  for ( QStringList::Iterator it = anAddFamilies.begin(); it != anAddFamilies.end(); ++it )
  {
    aFamily = *it;
    if ( famdb.contains(aFamily) )
    {
      f.setFamily( aFamily );
      aResMgr->setValue( "PyConsole", "font", f );
      break;
    }
  }
}

/*!Destructor.
 *\li Save window geometry.
 *\li Save desktop geometry.
 *\li Save resource maneger.
 *\li Delete selection manager.
 */
LightApp_Application::~LightApp_Application()
{
  savePreferences();
  delete mySelMgr;
  delete myScreenHelper;
  myPrefs = 0;
}

/*!Start application.*/
void LightApp_Application::start()
{
  CAM_Application::start();

  updateWindows();
  updateViewManagers();
  updateCommandsStatus();

  putInfo( "" );
  desktop()->statusBar()->showMessage( "" );

  LightApp_EventFilter::Init();

  onNewDoc();
}

/*!Closeapplication.*/
void LightApp_Application::closeApplication()
{
#ifndef DISABLE_QTXWEBBROWSER
  QProcess::startDetached( "HelpBrowser",
                           QStringList() << QString( "--remove=%1" ).arg( QApplication::instance()->applicationPid() ) );
#endif
  CAM_Application::closeApplication();
}

/*!Gets application name.*/
QString LightApp_Application::applicationName() const
{
  static QString _app_name;
  if ( _app_name.isEmpty() )
    _app_name = tr( "APP_NAME" );
  return _app_name;
}

/*!Gets application version.*/
QString LightApp_Application::applicationVersion() const
{
  static QString _app_version;

  if ( _app_version.isEmpty() )
  {
    QString resVersion = tr( "APP_VERSION" );
    if ( resVersion != "APP_VERSION" )
    {
      _app_version = resVersion;
    }
    else
    {
      _app_version = GUI_VERSION_STR;
    }
  }
  return _app_version;
}

/*!Load module by \a name.*/
CAM_Module* LightApp_Application::loadModule( const QString& name, const bool showMsg )
{
  CAM_Module* mod = CAM_Application::loadModule( name, showMsg );
  if ( mod )
  {
    connect( this, SIGNAL( studyOpened() ), mod, SLOT( onModelOpened() ) );
    connect( this, SIGNAL( studySaved() ),  mod, SLOT( onModelSaved() ) );
    connect( this, SIGNAL( studyClosed() ), mod, SLOT( onModelClosed() ) );
  }
  return mod;
}

/*!Activate module by \a modName*/
bool LightApp_Application::activateModule( const QString& modName )
{
  QString actName;
  CAM_Module* prevMod = activeModule();

  if ( prevMod )
    actName = prevMod->moduleName();

  QString name = modName;
  if ( !name.isEmpty() && !moduleTitle( modName ).isEmpty() )
    name = moduleTitle( modName );

  if ( actName == name )
    return true;

  putInfo( tr( "ACTIVATING_MODULE" ).arg( name ) );

  saveDockWindowsState();

  if ( infoPanel() )
    infoPanel()->clear();

  bool status = CAM_Application::activateModule( name );

  updateModuleActions();

  putInfo( "" );

  if ( !status )
    return false;

  updateWindows();
  updateViewManagers();

  if ( activeStudy() && activeStudy()->root() && objectBrowser() ) {
    if ( objectBrowser()->root() != activeStudy()->root() )
      objectBrowser()->setRoot( activeStudy()->root() );
    updateObjectBrowser( true );
  }

  if ( activeModule() ) activeModule()->updateModuleVisibilityState();

  updateActions();
  return true;
}

/*!Gets selection manager.*/
LightApp_SelectionMgr* LightApp_Application::selectionMgr() const
{
  return mySelMgr;
}

/*!Creat action "New window" for certain type of viewer:*/
void LightApp_Application::createActionForViewer( const int id,
                                                  const int parentId,
                                                  const QString& suffix,
                                                  const int accel )
{
  QString vtlt = tr( QString( "NEW_WINDOW_%1" ).arg( suffix ).toLatin1().constData() );
  QString tip = tr( "CREATING_NEW_WINDOW" ).arg( vtlt.remove( "&" ) );
  QAction* a = createAction( id,                      // menu action id
                             tip,                     // status tip
                             QIcon(),                 // icon
                             vtlt,                    // menu text
                             tip,                     // tooltip
                             accel,                   // shortcut
                             desktop(),               // parent
                             false,                   // toggle flag
                             this,                    // receiver
                             SLOT( onNewWindow() ) ); // slot
  createMenu( a, parentId, -1 );
}

/*!Create actions:*/

void LightApp_Application::createActions()
{
  CAM_Application::createActions();

  SUIT_Desktop* desk = desktop();
  SUIT_ResourceMgr* resMgr = resourceMgr();

  // Preferences
  createAction( PreferencesId, tr( "TOT_DESK_PREFERENCES" ), QIcon(),
                tr( "MEN_DESK_PREFERENCES" ), tr( "PRP_DESK_PREFERENCES" ),
                Qt::CTRL+Qt::Key_P, desk, false, this, SLOT( onPreferences() ) );

  // Help menu

  int helpMenu = createMenu( tr( "MEN_DESK_HELP" ), -1, -1, 1000 );

  QString url;

  // a) Link to web site
  if ( HAS_WWW_URL ) {
    url = resMgr->stringValue("GUI", "site_url");
    if ( !url.isEmpty() ) {
      QString title = tr ( "SALOME_SITE" );
      QAction* as = createAction( WebSiteId, title,
                                  resMgr->loadPixmap( "LightApp", tr( "ICON_WWW" ), false ),
                                  title, title,
                                  0, desk, false, this, SLOT( onHelpContentsModule() ) );
      as->setData( url );
      createMenu( as, helpMenu, -1, 0 );
    }
  }

  // b) Link to Forum
  if ( HAS_FORUM_URL ) {
    url = resMgr->stringValue("GUI", "forum_url");
    if ( !url.isEmpty() ) {
      QString title = tr ( "SALOME_FORUM" );
      QAction* af = createAction( ForumId, title,
                                  resMgr->loadPixmap( "LightApp", tr( "ICON_WWW" ), false ),
                                  title, title,
                                  0, desk, false, this, SLOT( onHelpContentsModule() ) );
      af->setData( url );
      createMenu( af, helpMenu, -1, 0 );
    }
  }

  // c) Link to YouTube channel
  if ( HAS_YOUTUBE_URL ) {
    url = resMgr->stringValue("GUI", "channel_url");
    if ( !url.isEmpty() ) {
      createMenu( separator(), helpMenu, -1, 0 );
      QString title = tr ( "SALOME_VIDEO_TUTORIALS" );
      QAction* av = createAction( VideosId, title,
                                  resMgr->loadPixmap( "LightApp", tr( "ICON_LIFE_RIGN" ), false ),
                                  title, tr( "PRP_SALOME_VIDEO_TUTORIALS" ),
                                  0, desk, false, this, SLOT( onHelpContentsModule() ) );
      av->setData( url );
      createMenu( av, helpMenu, -1, 0 );
    }
  }

  // d) Link to Tutorials
  if ( HAS_TUTORIAL_URL ) {
    url = resMgr->stringValue("GUI", "tutorials_url");
    if ( !url.isEmpty() ) {
      QString title = tr ( "SALOME_TUTORIALS" );
      QAction* as = createAction( TutorialsId, title,
                                  resMgr->loadPixmap( "LightApp", tr( "ICON_WWW" ), false ),
                                  title, tr( "PRP_SALOME_TUTORIALS" ),
                                  0, desk, false, this, SLOT( onHelpContentsModule() ) );
      as->setData( url );
      createMenu( as, helpMenu, -1, 0 );
    }
  }

  // e) Help for modules

  QStringList aModuleList;
  modules( aModuleList, false );
  aModuleList.prepend( "GUI" );
  aModuleList.prepend( "KERNEL" );

  foreach( QString aModule, aModuleList )
    createHelpItems( aModule );

  // f) Additional help items

  int id = LightApp_Application::UserID + FIRST_HELP_ID + 1000;
  createMenu( separator(), helpMenu, -1, 10 );

  QStringList addHelpItems = resMgr->parameters( "add_help" );
  foreach ( QString paramName, addHelpItems ) {
    QString helpItem = getHelpItem( "add_help", paramName );
    if ( !helpItem.isEmpty() )
    {
      QPixmap helpIcon = helpItem.startsWith( "http", Qt::CaseInsensitive ) ?
        resMgr->loadPixmap( "STD", tr( "ICON_WWW" ), false ) : resMgr->loadPixmap( "STD", tr( "ICON_HELP" ), false );
      QAction* a = createAction( id++, paramName, helpIcon, paramName, paramName,
                                 0, desk, false, this, SLOT( onHelpContentsModule() ) );
      a->setData( helpItem );
      createMenu( a, helpMenu, -1, 10 );
    }
  }

  //! MRU
  static QtxMRUAction* mru = new QtxMRUAction( tr( "TOT_DESK_MRU" ), tr( "MEN_DESK_MRU" ), 0 );
  connect( mru, SIGNAL( activated( const QString& ) ), this, SLOT( onMRUActivated( const QString& ) ) );
  registerAction( MRUId, mru );

  // List of modules
  LightApp_ModuleAction* moduleAction = new LightApp_ModuleAction( resMgr, desk );
  registerAction( ModulesListId, moduleAction );
  // a. here we add regular modules (specified to GUI via --modules cmd line option, or default list from configuration)
  // b. custom modules are added in customize() method
  QStringList modList;
  modules( modList, false );
  foreach ( QString aModule, modList )
    moduleAction->insertModule( aModule, moduleIcon( aModule, 20 ) ); // scale icon to 20x20 pix

  connect( this, SIGNAL( moduleActivated( QString ) ),
           moduleAction, SLOT( setActiveModule( QString ) ) );
  connect( moduleAction, SIGNAL( moduleActivated( const QString& ) ),
           this, SLOT( onModuleActivation( const QString& ) ) );
  connect( moduleAction, SIGNAL( adding() ),
           this, SLOT( onExtAdding() ) );
  connect( moduleAction, SIGNAL( removing( QString ) ),
           this, SLOT( onExtRemoving( QString ) ) );
  connect( moduleAction, SIGNAL(showExtInfo()),
           this, SLOT(onShowExtInfo()));

  addExtensionsActions(moduleAction);

  // New window
  int windowMenu = createMenu( tr( "MEN_DESK_WINDOW" ), -1, MenuWindowId, 100 );
  int newWinMenu = createMenu( tr( "MEN_DESK_NEWWINDOW" ), windowMenu, -1, 0 );

  createAction( CloseId, tr( "TOT_CLOSE" ), QIcon(), tr( "MEN_DESK_CLOSE" ), tr( "PRP_CLOSE" ),
                Qt::CTRL+Qt::Key_F4, desk, false, this, SLOT( onCloseWindow() ) );
  createAction( CloseAllId, tr( "TOT_CLOSE_ALL" ), QIcon(), tr( "MEN_DESK_CLOSE_ALL" ), tr( "PRP_CLOSE_ALL" ),
                0, desk, false, this, SLOT( onCloseAllWindow() ) );
  createAction( GroupAllId, tr( "TOT_GROUP_ALL" ), QIcon(), tr( "MEN_DESK_GROUP_ALL" ), tr( "PRP_GROUP_ALL" ),
                0, desk, false, this, SLOT( onGroupAllWindow() ) );

  createMenu( CloseId,     windowMenu, 0, -1 );
  createMenu( CloseAllId,  windowMenu, 0, -1 );
  createMenu( GroupAllId,  windowMenu, 0, -1 );
  createMenu( separator(), windowMenu, -1, 0 );

#ifndef DISABLE_GLVIEWER
  createActionForViewer( NewGLViewId, newWinMenu, QString::number( 0 ), Qt::ALT+Qt::Key_G );
#endif
#ifndef DISABLE_PLOT2DVIEWER
  createActionForViewer( NewPlot2dId, newWinMenu, QString::number( 1 ), Qt::ALT+Qt::Key_P );
#endif
#ifndef DISABLE_OCCVIEWER
  createActionForViewer( NewOCCViewId, newWinMenu, QString::number( 2 ), Qt::ALT+Qt::Key_O );
#endif
#ifndef DISABLE_VTKVIEWER
  createActionForViewer( NewVTKViewId, newWinMenu, QString::number( 3 ), Qt::ALT+Qt::Key_K );
#endif
#ifndef DISABLE_QXGRAPHVIEWER
  createActionForViewer( NewQxSceneViewId, newWinMenu, QString::number( 4 ), Qt::ALT+Qt::Key_S );
#endif
#ifndef DISABLE_GRAPHICSVIEW
  createActionForViewer( NewGraphicsViewId, newWinMenu, QString::number( 5 ), Qt::ALT+Qt::Key_C );
#endif
#ifndef DISABLE_PVVIEWER
  createActionForViewer( NewPVViewId, newWinMenu, QString::number( 6 ), Qt::ALT+Qt::Key_A );
#endif
#ifndef DISABLE_PYVIEWER
  createActionForViewer( NewPyViewerId, newWinMenu, QString::number( 7 ), Qt::ALT+Qt::Key_Y );
#endif
#ifndef DISABLE_PV3DVIEWER
  createActionForViewer( NewPV3DViewId, newWinMenu, QString::number( 8 ), Qt::ALT+Qt::Key_3 );
#endif

  createAction( RenameId, tr( "TOT_RENAME" ), QIcon(), tr( "MEN_DESK_RENAME" ), tr( "PRP_RENAME" ),
                Qt::ALT+Qt::SHIFT+Qt::Key_R, desk, false, this, SLOT( onRenameWindow() ) );
  createMenu( RenameId, windowMenu, -1 );

  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1 );
  createMenu( PreferencesId, fileMenu, 50, -1 );
  createMenu( separator(), fileMenu, -1, 50, -1 );

  createMenu( separator(), fileMenu, -1, 100, -1 );
  createMenu( MRUId, fileMenu, 100, -1 );
  createMenu( separator(), fileMenu, -1, 100, -1 );

#ifdef USE_SALOME_STYLE
  createAction( StyleId, tr( "TOT_THEME" ), QIcon(), tr( "MEN_DESK_THEME" ), tr( "PRP_THEME" ),
                0, desk, false, this, SLOT( onStylePreferences() ) );
#endif // USE_SALOME_STYLE

  createAction( FullScreenId, tr( "TOT_FULLSCREEN" ), QIcon(), tr( "MEN_DESK_FULLSCREEN" ), tr( "PRP_FULLSCREEN" ),
                Qt::Key_F11, desk, false, this, SLOT( onFullScreen() ) );


  int viewMenu = createMenu( tr( "MEN_DESK_VIEW" ), -1 );
  createMenu( separator(), viewMenu, -1, 20, -1 );
#ifdef USE_SALOME_STYLE
  createMenu( StyleId, viewMenu, 20, -1 );
#endif // USE_SALOME_STYLE
  createMenu( FullScreenId, viewMenu, 20, -1 );
  createMenu( separator(), viewMenu, -1, 20, -1 );
  createMenu( ModulesListId, viewMenu );

  int modTBar = createTool( tr( "INF_TOOLBAR_MODULES" ),    // title (language-dependant)
                            QString( "SalomeModules" ) );   // name (language-independant)
  createTool( ModulesListId, modTBar );
}

/*!Create actions for installed extensions:*/
void LightApp_Application::addExtensionsActions(LightApp_ModuleAction* moduleAction)
{
  if (!moduleAction)
  {
    MESSAGE("Couldn't get a moduleAction! Return.");
    return;
  }

  // It should be set on the app start if we use an --on_demand 1 command line option
  auto extRootDir = getenv(salomeAppDir);
  if (!extRootDir)
  {
    // It's ok if we don't use --on_demand
    return;
  }
  SCRUTE(extRootDir);

   // Import Python module that manages SALOME extensions.
  PyLockWrapper lck; // acquire GIL
  PyObjWrapper extensionQuery = PyImport_ImportModule((char*)"SalomeOnDemandTK.extension_query");
  PyObjWrapper installedExtensions = PyObject_CallMethod(
      extensionQuery, (char*)"ext_by_name", (char*)"s", extRootDir);
  if (!installedExtensions)
  {
    return;
  }

  // Iterate installed extensions
  for (Py_ssize_t pos = 0; pos < PyList_Size(installedExtensions); ++pos)
  {
    // Get the current ext name
    auto extNameItem = PyList_GetItem(installedExtensions, pos);
    QString extName(PyUnicode_AsUTF8(extNameItem));
    SCRUTE(extName.toStdString());

    moduleAction->insertExtension(extName);
  }

  // Udate actions only once after all of them were already inserted
  moduleAction->updateExtActions();
}

/*!
  Customize actions.
*/
void LightApp_Application::customize()
{
  // List of modules
  LightApp_ModuleAction* moduleAction = qobject_cast<LightApp_ModuleAction*>( action( ModulesListId ) );
  // a. regular modules were added in createActions() method
  // b. here we add custom modules (manually added by the user)
  if ( HAS_SALOME_ON_DEMAND && QString::compare(getenv("SALOME_ON_DEMAND"),"HIDE", Qt::CaseInsensitive) != 0)
  {
    // Update rc file
    updateSalomeApprc();

    QStringList modList = resourceMgr()->stringValue( "launch", "user_modules" ).split( ";", QString::SkipEmptyParts );
    foreach ( QString aModule, modList )
      addUserModule(  aModule, resourceMgr()->stringValue( "user_modules", aModule ), true );
  }
  else
  {
    moduleAction->setModeEnabled( LightApp_ModuleAction::AddRemove, false );
  }
}

/*!
  Update rc file with SALOME_APPLICATION_DIR or with SALOME_MODULES.
*/
void LightApp_Application::updateSalomeApprc()
{
    SUIT_ResourceMgr* resMgr = resourceMgr();
    auto extRootDir = getenv(salomeAppDir);

    QString salomemodules(getenv("SALOME_MODULES"));
    if(salomemodules.isEmpty())
        UpdateCompInfo_with_salomeappdir(  QDir(extRootDir), resMgr );
    else
        UpdateCompInfo_with_salomemodules(salomemodules, QDir(extRootDir), resMgr);
}

/*!On module activation action.*/
void LightApp_Application::onModuleActivation( const QString& modTitle )
{
  // Force user to create/open a study before module activation
  QPixmap icon = moduleIcon( modTitle );
  bool cancelled = false;

  while ( !modTitle.isEmpty() && !activeStudy() && !cancelled ){
    LightApp_ModuleDlg aDlg( desktop(), modTitle, icon );
    QMap<int, QString> opmap = activateModuleActions();
    for ( QMap<int, QString>::ConstIterator it = opmap.begin(); it != opmap.end(); ++it )
      aDlg.addButton( it.value(), it.key() );

    int res = aDlg.exec();
    if ( res ) {
      // some operation is selected
      moduleActionSelected( res );
    }
    else {
      // cancelled
      putInfo( tr("INF_CANCELLED") );
      emit moduleActivated( QString() );
      cancelled = true;
    }
  }

  if ( !cancelled )
    activateModule( modTitle );
}

/*!On extension adding action.*/
void LightApp_Application::onExtAdding()
{
  // Show dialog to browse a salome extension file
  QStringList filters = (QStringList() << tr("Salome extension files") + " (*.salomex)" << tr("All files") + " (*)");
  QStringList paths = getOpenFileNames(QString(), filters.join(";;"), QString(), desktop());
  if (paths.isEmpty())
  {
    MESSAGE("Adding an extension was cancelled.");
    return;
  }

  LightApp_ModuleAction* moduleAction = qobject_cast<LightApp_ModuleAction*>(action(ModulesListId));
  if (!moduleAction)
  {
    MESSAGE("Couldn't get a moduleAction! Return.");
    return;
  }

  // It should be set on the app start
  auto extRootDir = getenv(salomeAppDir);
  if (!extRootDir)
  {
    SUIT_MessageBox::warning(desktop(), tr("WRN_WARNING"), tr("WRN_SALOME_APPLICATION_DIR"));
    return;
  }
  SCRUTE(extRootDir);

  // We'll load all the extensions modules from this path
  auto SalomeExtDir = QDir::cleanPath(QString(extRootDir) + QDir::separator() + "__SALOME_EXT__");
  SCRUTE(SalomeExtDir.toStdString());

  // Import Python module that manages SALOME extensions.
  // It seems to be faster to lock and unlock once than on each iteration,
  // but I didn't compare the performance for each case.
  PyLockWrapper lck; // acquire GIL
  PyObjWrapper extensionUnpacker = PyImport_ImportModule((char*)"SalomeOnDemandTK.extension_unpacker");
  PyObjWrapper runSalomeOnDemand = PyImport_ImportModule((char*)"runSalomeOnDemand");

  // Loop via selected extensions files
  foreach(QString path, paths)
  {
    std::string extPath = path.toStdString();
    SCRUTE(extPath);

    PyObjWrapper unpackedModules = PyObject_CallMethod(
      extensionUnpacker, (char*)"install_salomex", (char*)"s", extPath.c_str());
    if (!unpackedModules || unpackedModules == Py_None)
    {
      SUIT_MessageBox::warning(desktop(), tr("WRN_WARNING"), tr("WRN_FAILED_UNPACK_EXTENSION").arg(path) );
      continue;
    }

    PyObjWrapper pKeys = PyDict_Keys(unpackedModules);
    // Iterate all the components (modules) for this extension
    for (Py_ssize_t pos = 0; pos < PyDict_Size(unpackedModules); ++pos)
    {
      auto moduleNameItem = PyList_GetItem(pKeys, pos);
      auto interactiveItem = PyDict_GetItem(unpackedModules, moduleNameItem);

      QString moduleName(PyUnicode_AsUTF8(moduleNameItem));
      SCRUTE(moduleName.toStdString());
      addUserModule(moduleName, SalomeExtDir, PyObject_IsTrue(interactiveItem));
    }

    // Add an extension to GUI
    QFileInfo extFileInfo(path);
    QString extName = extFileInfo.baseName();
    if (moduleAction)
    {
      moduleAction->insertExtension(extName);
    }

    // Update environment of salome
    PyObjWrapper update_env = PyObject_CallMethod(
      runSalomeOnDemand, (char*)"set_selext_env", (char*)"ss", extRootDir, extName.toStdString().c_str());
    if (!update_env)
    {
      SUIT_MessageBox::warning(desktop(), tr("WRN_WARNING"), tr("WRN_FAILED_UPDATE_ENV").arg(extName + "_env.py") );
      continue;
    }
  }

  // Udate actions only once after all of them were already inserted
  moduleAction->updateExtActions();
}

/*Add user module.*/
bool LightApp_Application::addUserModule( const QString& name, const QString& root, bool interactive )
{
  if ( name == "KERNEL" || name == "GUI" )
    return false; // skip KERNEL and GUI modules

  if ( name.isEmpty() || root.isEmpty() )
    return false;

  if ( !moduleTitle( name ).isEmpty() ) // module alread in current session
  {
    if ( interactive )
      SUIT_MessageBox::warning( desktop(), tr( "WRN_WARNING" ), tr( "WRN_MODULE_DUPLICATED" ).arg( name ) );
    return false;
  }
  if ( !QFileInfo( root ).exists() ) // root directory does not exist
  {
    if ( interactive )
      SUIT_MessageBox::warning( desktop(), tr( "WRN_WARNING" ), tr( "WRN_MODULE_ROOT_DOES_NOT_EXIST" ).arg( root ) );
    return false;
  }
  // resources directory
  QString resDir = Qtx::joinPath( QStringList() << root << "share" << "salome" << "resources" << name.toLower() );
  if ( !QFileInfo( resDir ).exists() ) // resources directory does not exist
  {
    if ( interactive )
      SUIT_MessageBox::warning( desktop(), tr( "WRN_WARNING" ), tr( "WRN_MODULE_BAD_RESDIR" ).arg( resDir ) );
    return false;
  }

  SUIT_ResourceMgr* resMgr = resourceMgr();

  // read XML configuration file
  resMgr->setConstant(QString("%1_ROOT_DIR").arg(name), root);
  if (!resMgr->addResource(resDir)) // cannot read configuration
  {
    if ( interactive )
      SUIT_MessageBox::warning( desktop(), tr( "WRN_WARNING" ), tr( "WRN_MODULE_CANNOT_READ_CFG" ).arg( resDir ) );
    return false;
  }
  // fill in information about module
  if ( !appendModuleInfo( name ) ) // cannot append module information to internal table
  {
    if ( interactive )
      SUIT_MessageBox::warning( desktop(), tr( "WRN_WARNING" ), tr( "WRN_MODULE_BAD_CFG_FILE" ).arg( name ) );
    return false;
  }

  // load translations
  resMgr->loadLanguage(name);

  // Do all the GUI related stuff only if the module supports that.
  // We already did check for GUI inside CAM_Application::appendModuleInfo, but
  // need to do that again.
  // TODO: Maybe it's better to return ModuleInfo from appendModuleInfo() and check status.
  const QString title = resMgr->stringValue(name, "name", QString()).trimmed();
  if (resMgr->booleanValue(name, "gui", false) || !title.isEmpty() && interactive)
  {
    // Append module to the menu / toolbar
    LightApp_ModuleAction* moduleAction = qobject_cast<LightApp_ModuleAction*>(action(ModulesListId));
    if (moduleAction)
    {
      // Scale icon to 20x20 pix
      moduleAction->insertModule(moduleTitle(name), moduleIcon(moduleTitle(name), 20), true);
    }
  }

  // add empty page to Preferences dialog
  LightApp_Preferences* prefs = preferences();
  if ( prefs && !prefs->hasModule( moduleTitle( name ) ) )
  {
    int prefId = prefs->addPreference( moduleTitle( name ) );
    prefs->setItemIcon( prefId, moduleIcon( moduleTitle( name ), 20 ) ); // scale icon to 20x20 pix
    LightApp_Module* m = qobject_cast<LightApp_Module*>( module( moduleTitle( name ) ) );
    if ( m )
    {
      m->createPreferences();
      emptyPreferences( moduleTitle( name ) );
    }
  }
  // add Help items
  createHelpItems( moduleTitle( name ) );
  // extend module catalog
  QString catalogue = QDir( resDir ).filePath( QString( "%1Catalog.xml" ).arg( name ) );
  addCatalogue( name, catalogue );
  // update windows (in particular, Info panel)
  updateWindows();
  // save module in the resource manager
  if ( interactive )
  {
    QStringList customModules = resMgr->stringValue("launch", "user_modules").split(";", QString::SkipEmptyParts);
    customModules << name;
    customModules.removeDuplicates();
    resMgr->setValue( "launch", "user_modules", customModules.join( ";" ) );
    resMgr->setValue( "user_modules", name, root );
  }
  return true;
}

/*!Remove user module from UI.*/
void LightApp_Application::removeUserModule(const QString& moduleInnerName, LightApp_ModuleAction* moduleAction)
{
  MESSAGE("Remove a module from UI...");
  SCRUTE(moduleInnerName.toStdString());

  // There is a some confusion point, because now we have a module's 'inner' name
  // from the extension's salomexd file.
  // But, in the next GUI methods we need to use a module title (user name).
  // For example, PYHELLO (inner name) and PyHello (user name to display in GUI).
  // Then, from the inner module's name we need to get a user one.
  QString moduleUserName = moduleTitle(moduleInnerName);
  SCRUTE(moduleUserName.toStdString());

  // Set current state in modules combo box
  // Don't confuse again, because activeModule()->moduleName() returns a module title, not an inner one!
  if (activeModule() && activeModule()->moduleName() == moduleUserName)
    activateModule("");

  // Remove from "Modules" menu and toolbar
  if (moduleAction)
  {
    moduleAction->removeModule(moduleUserName);
  }

  // Remove Help menu items
  removeHelpItems(moduleUserName);

  // Remove Preferences
  LightApp_Preferences* prefs = preferences();
  if (prefs)
    prefs->removeModule(moduleUserName);

  // Remove settings
  // Here we use an inner module name!
  QStringList customModules = resourceMgr()->stringValue("launch", "user_modules").split(";", QString::SkipEmptyParts);
  customModules.removeAll(moduleInnerName);
  resourceMgr()->setValue("launch", "user_modules", customModules.join(";"));
  removeModuleInfo(moduleInnerName);
}

/*!On module removing action.*/
void LightApp_Application::onExtRemoving(const QString& title)
{
  MESSAGE("Remove an extension...");
  std::string extName = title.toStdString();
  SCRUTE(extName);

  // Ask user if he's ready to completely remove an extension and all its modules.
  int answer = SUIT_MessageBox::question(
    desktop(),
    tr("TLT_REMOVE_EXTENSION"),
    tr("QUE_REMOVE_EXTENSION").arg(title),
    SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel,
    SUIT_MessageBox::Ok
  );

  if (answer == SUIT_MessageBox::Cancel)
  {
    MESSAGE("Removing of an extension was cancelled");
    return; // cancelled
  }
  
  if (activeStudy() && activeStudy()->isModified() && !onSaveDoc())
  {
    // doc is not saved, or saving cancelled
    SUIT_MessageBox::warning(
      desktop(),
      tr("WRN_WARNING"), tr("WRN_CANCEL_REMOVE_EXTENSION_UNSAVE").arg(title)
    );

    return;
  }

  // It should be set on the app start
  auto extRootDir = getenv(salomeAppDir);
  if (!extRootDir)
  {
    SUIT_MessageBox::warning(desktop(), tr("WRN_WARNING"), tr("WRN_SALOME_APPLICATION_DIR"));
    return;
  }
  SCRUTE(extRootDir);

  // Import Python module that manages SALOME extensions.
  PyLockWrapper lck; // acquire GIL
  PyObjWrapper extensionRemover = PyImport_ImportModule((char*)"SalomeOnDemandTK.extension_remover");
  PyObjWrapper removedModules = PyObject_CallMethod(
      extensionRemover, (char*)"remove_salomex", (char*)"ss", extRootDir, extName.c_str());
  if (!removedModules || removedModules == Py_None)
  {
    SUIT_MessageBox::warning(desktop(), tr("WRN_WARNING"), tr("WRN_FAILED_REMOVE_EXTENSION").arg(title));
    return;
  }

  // We need it to remove ext and modules from UI
  LightApp_ModuleAction* moduleAction = qobject_cast<LightApp_ModuleAction*>(action(ModulesListId));
  if (!moduleAction)
  {
    MESSAGE("Cannot get a pointer to LightApp_ModuleAction! Removing from menue and toolbars will skipped.");
  }

  // Module's content was already removed on python remove_salomex call,
  // then all we do next - just remove UI items.
  for (Py_ssize_t pos = 0; pos < PyList_Size(removedModules); ++pos)
  {
    // Get the current module's name
    auto moduleNameItem = PyList_GetItem(removedModules, pos);
    const QString moduleInnerName(PyUnicode_AsUTF8(moduleNameItem));

    removeUserModule(moduleInnerName, moduleAction);
  }

  // Remove an ext from UI
  if (moduleAction)
  {
    moduleAction->removeExtension(title);
  }

  // Update windows (in particular, Info panel)
  updateWindows();
}

/*!On show extension info action.*/
void LightApp_Application::onShowExtInfo()
{
  // Show dialog with information about loaded salome extensions
  LightApp_ExtInfoDlg dlg(desktop());
  dlg.exec();
}

/*!Default module activation.*/
QString LightApp_Application::defaultModule() const
{
  QStringList aModuleNames;
  modules( aModuleNames, false ); // obtain a complete list of module names for the current configuration
  //! If there's the one and only module --> activate it automatically
  //! TODO: Possible improvement - default module can be taken from preferences
  return aModuleNames.count() > 1 ? "" : ( aModuleNames.count() ? aModuleNames.first() : "" );
}

/*!On new window slot.*/
void LightApp_Application::onNewWindow()
{
  const QObject* obj = sender();
  if ( !obj || !obj->inherits( "QAction" ) )
    return;

  QString type;
  int id = actionId( (QAction*)obj );
  switch ( id )
  {
#ifndef DISABLE_GLVIEWER
  case NewGLViewId:
    type = GLViewer_Viewer::Type();
    break;
#endif
#ifndef DISABLE_PLOT2DVIEWER
  case NewPlot2dId:
    type = Plot2d_Viewer::Type();
    break;
#endif
#ifndef DISABLE_OCCVIEWER
  case NewOCCViewId:
    type = OCCViewer_Viewer::Type();
    break;
#endif
#ifndef DISABLE_VTKVIEWER
  case NewVTKViewId:
    type = VTKViewer_Viewer::Type();
    break;
#endif
#ifndef DISABLE_QXGRAPHVIEWER
  case NewQxSceneViewId:
    type = QxScene_Viewer::Type();
    break;
#endif
#ifndef DISABLE_GRAPHICSVIEW
  case NewGraphicsViewId:
    type = GraphicsView_Viewer::Type();
    break;
#endif
#ifndef DISABLE_PVVIEWER
  case NewPVViewId:
    type = PVViewer_Viewer::Type();
    break;
#endif
#ifndef DISABLE_PYVIEWER
  case NewPyViewerId:
    type = PyViewer_Viewer::Type();
    break;
#endif
#ifndef DISABLE_PV3DVIEWER
  case NewPV3DViewId:
    type = PV3DViewer_ViewModel::Type();
    break;
#endif
  }

  if ( !type.isEmpty() )
    createViewManager( type );
}

/*!
  SLOT: Creates new document
*/
void LightApp_Application::onNewDoc()
{
  if ( !checkExistingDoc() )
    return;

  //asl: fix for 0020515
  saveDockWindowsState();

  CAM_Application::onNewDoc();
}

/*!
  SLOT: Opens new document
*/
void LightApp_Application::onOpenDoc()
{
  SUIT_Study* study = activeStudy();

  if ( !checkExistingDoc( false ) )
    return;

  QString aName = getFileName( true, QString(), getFileFilter( true ), QString(), 0 );
  if ( aName.isNull() ) //Cancel
    return;

  onOpenDoc( aName );

  if ( !study ) // new study will be create in THIS application
  {
    updateWindows();
    updateViewManagers();
  }
}

bool LightApp_Application::canOpenDoc( const QString& )
{
  return true;
}

/*!
  SLOT: Opens new document.
  \param aName - name of file
*/
bool LightApp_Application::onOpenDoc( const QString& aName )
{
  if ( !canOpenDoc(aName)) {
    bool showError = !property("open_study_from_command_line").isValid() ||
      !property("open_study_from_command_line").toBool();

    putInfo( tr("OPEN_DOCUMENT_PROBLEM") );
    if ( showError )
      SUIT_MessageBox::critical( desktop(), tr("ERR_ERROR"), tr("OPEN_DOCUMENT_PROBLEM"));

    return false;
  }

  closeDoc(false);

  if ( !checkExistingDoc() )
    return false;

  saveDockWindowsState();

  // We should take mru action first because this application instance can be deleted later.
  QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );

  bool res = CAM_Application::onOpenDoc( aName );

  if ( mru )
  {
    if ( res )
      mru->insert( aName );
    else
      mru->remove( aName );
  }
  return res;
}

/*!
  SLOT: Displays "About" message box
*/
void LightApp_Application::onHelpAbout()
{
  LightApp_AboutDlg dlg( applicationName(), applicationVersion(), desktop() );
  dlg.exec();
}

/*!
  Private SLOT: Called on selection is changed
  Dispatchs active module that selection is changed
*/
void LightApp_Application::onSelection()
{
  //MESSAGE("onSelection")
  onSelectionChanged();

  if ( activeModule() && activeModule()->inherits( "LightApp_Module" ) )
    ((LightApp_Module*)activeModule())->selectionChanged();
}

/*!
  Sets active study.
 \param study - SUIT_Study.
*/
void LightApp_Application::setActiveStudy( SUIT_Study* study )
{
  CAM_Application::setActiveStudy( study );
}

/*!
  Enables/Disables menu items and toolbar buttons. Rebuild menu
*/
void LightApp_Application::updateCommandsStatus()
{
  CAM_Application::updateCommandsStatus();
  QAction* a = 0;

#ifndef DISABLE_GLVIEWER
  a = action( NewGLViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_PLOT2DVIEWER
  a = action( NewPlot2dId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_OCCVIEWER
  a = action( NewOCCViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_VTKVIEWER
  a = action( NewVTKViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_QXGRAPHVIEWER
  a = action( NewQxSceneViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_GRAPHICSVIEW
  a = action( NewGraphicsViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_PVVIEWER
  a = action( NewPVViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_PYVIEWER
  a = action( NewPyViewerId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_PV3DVIEWER
  a = action( NewPV3DViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif
}

/*!
  \class RunBrowser
  Runs system command in separate thread
*/
class RunBrowser: public QThread
{
public:
  static void execute( LightApp_Application* application,
                       const QString& browser,
                       const QString& parameters,
                       const QString& url )
  {
    (new RunBrowser( application, browser, parameters, url ))->start();
  }

protected:
  RunBrowser( LightApp_Application* application,
              const QString&        browser,
              const QString&        parameters,
              const QString&        url)
    : myApplication( application ),
      myBrowser( browser ),
      myParameters( parameters ),
      myUrl( url )
  {
    if ( !myUrl.startsWith( "http", Qt::CaseInsensitive ) )
    {
      // normalize path
      if ( myUrl.startsWith( "file://", Qt::CaseInsensitive ) )
        myUrl = myUrl.remove( 0, QString( "file://" ).count() );
      // For the external browser we always specify 'file://' protocol,
      // because some web browsers (e.g. Mozilla Firefox) can't open local file without protocol.
      myUrl = myUrl.prepend( "file://" );
    }
    connect(this, SIGNAL(finished()), SLOT(deleteLater()));
  }

  virtual void run()
  {
    if ( !myBrowser.isEmpty() && !myUrl.isEmpty() )
    {
      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef WIN32
      QString cmdLine = QString( "\"%1\" %2 \"%3\"" ).arg( myBrowser, myParameters, myUrl );
#else
      QString cmdLine = QString( "%1 %2 \"%3\"" ).arg( myBrowser, myParameters, myUrl );
      // remove LD_LIBRARY_PATH from the environement before starting launcher to avoid bad interactions.
      // (especially in the case of universal binaries)
      env.remove("LD_LIBRARY_PATH");
#endif
      QProcess* proc = new QProcess();
      proc->setProcessEnvironment(env);
      proc->start( cmdLine );
      if ( !proc->waitForStarted() )
      {
        SALOME_CustomEvent* ce2000 = new SALOME_CustomEvent( 2000 );
        QString* msg = new QString( QObject::tr( "EXTERNAL_BROWSER_CANNOT_SHOW_PAGE" ).arg( myBrowser, myUrl ) );
        ce2000->setData( msg );
        QApplication::postEvent( myApplication, ce2000 );
      }
    }
  }

private:
  LightApp_Application* myApplication;
  QString               myBrowser;
  QString               myParameters;
  QString               myUrl;
};

void LightApp_Application::showHelp( const QString& path )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();

#if DISABLE_QTXWEBBROWSER
  bool useExternalBrowser = true;
#else
  bool useExternalBrowser = resMgr->booleanValue("ExternalBrowser", "use_external_browser", false );
#endif

  if ( useExternalBrowser )
  {
#ifdef WIN32
    QString browser = resMgr->stringValue( "ExternalBrowser", "winapplication" ) ;
#else
    QString browser = resMgr->stringValue( "ExternalBrowser", "application" );
#endif
    QString parameters = resMgr->stringValue("ExternalBrowser", "parameters");

    if ( !browser.isEmpty() )
    {
      RunBrowser::execute( this, browser, parameters, path );
    }
    else
    {
      if ( SUIT_MessageBox::question( desktop(), tr( "WRN_WARNING" ), tr( "DEFINE_EXTERNAL_BROWSER" ),
                                      SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                      SUIT_MessageBox::Yes ) == SUIT_MessageBox::Yes )
      {
        QStringList path;
        path << tr( "PREF_CATEGORY_SALOME" ) << tr( "PREF_TAB_GENERAL" )
             << tr( "PREF_GROUP_EXT_BROWSER" ) << tr( "PREF_APP" );
        showPreferences( path );
      }
    }
  }
  else
  {
    QStringList cmdLine;
    cmdLine << QString( "--language=%1" ).arg( resMgr->stringValue( "language", "language" ) );
    cmdLine << QString( "--add=%1" ).arg( QApplication::instance()->applicationPid() );
    cmdLine << path;
    QProcess::startDetached( "HelpBrowser", cmdLine );
  }
}

/*!
  SLOT: Displays help contents for choosen module
*/
void LightApp_Application::onHelpContentsModule()
{
  const QAction* a = (QAction*) sender();
  QString helpFile = a->data().toString();
  if ( !helpFile.isEmpty() )
    showHelp( helpFile );
}

/*!
  SLOT: Displays contextual help (e.g. for choosen dialog)
*/
void LightApp_Application::onHelpContextModule( const QString& component,
                                                const QString& url,
                                                const QString& context )
{
  QString path = url;
  if ( !url.startsWith( "http", Qt::CaseInsensitive ) )
  {
    // local file path
    QFileInfo fi( url );
    if ( fi.isRelative() && !component.isEmpty() )
    {
      QString rootDir = Qtx::getenv( (component + "_ROOT_DIR").toLatin1().constData() );
      if ( !rootDir.isEmpty() )
      {
        path = (QStringList() << rootDir << "share" << "doc" << "salome" << "gui" << component << url).join( QDir::separator() );
      }
    }
  }
  if ( !context.isEmpty() )
  {
    path += QString( "#%1" ).arg( context );
  }
  showHelp( path );
}

/*!
  Sets enable or disable some actions on selection changed.
*/
void LightApp_Application::onSelectionChanged()
{
  LightApp_Module* m = dynamic_cast<LightApp_Module*>( activeModule() );
  bool canCopy  = m ? m->canCopy() : false;
  bool canPaste = m ? m->canPaste() : false;

  action( EditCopyId )->setEnabled(canCopy);
  action( EditPasteId )->setEnabled(canPaste);
}

/*!
  SLOT: Performs some actions when dockable windows are triggered
*/
void LightApp_Application::onDockWindowVisibilityChanged( bool )
{
}

QWidget* LightApp_Application::dockWindow( const int id ) const
{
  QWidget* wid = 0;
  if ( myWin.contains( id ) )
    wid = myWin[id];
  return wid;
}

QDockWidget* LightApp_Application::windowDock( QWidget* wid ) const
{
  if ( !wid )
    return 0;

  QDockWidget* dock = 0;
  QWidget* w = wid->parentWidget();
  while ( w && !dock )
  {
    dock = ::qobject_cast<QDockWidget*>( w );
    w = w->parentWidget();
  }
  return dock;
}

void LightApp_Application::insertDockWindow( const int id, QWidget* wid )
{
  if ( !wid )
    return;

  if ( wid != dockWindow( id ) )
    removeDockWindow( id );

  myWin.insert( id, wid );

  QtxDockWidget* dock = new QtxDockWidget( true, desktop() );
  if ( id == WT_InfoPanel ) {
    // Info panel's position is strongly limited to the right area;
    // It is not movable and not floatable.
    dock->setAllowedAreas( Qt::RightDockWidgetArea );
    dock->setFeatures( QDockWidget::DockWidgetClosable );
    connect( dock, SIGNAL( aboutToShow()), this, SLOT( onInfoPanelShown() ) );
  }
  else {
    dock->setFeatures( QDockWidget::AllDockWidgetFeatures );
  }
  connect( dock, SIGNAL(  destroyed( QObject* ) ), this, SLOT( onWCDestroyed( QObject* ) ) );

  dock->setObjectName( wid->objectName().isEmpty() ? QString( "window_%1" ).arg( id ) :
                       QString( "%1Dock" ).arg( wid->objectName() ) );
  dock->setWidget( wid );
  dock->toggleViewAction()->setData( QVariant( wid->objectName() ) );
  connect( dock->toggleViewAction(), SIGNAL( triggered( bool ) ),
           this, SLOT( onDockWindowVisibilityChanged( bool ) ) );

  QKeySequence accel = wid->property( "shortcut" ).value<QKeySequence>();
  if ( !accel.isEmpty() )
    dock->toggleViewAction()->setShortcut( accel );

  dock->show();
}

void LightApp_Application::removeDockWindow( const int id )
{
  QWidget* wid = dockWindow( id );
  if ( !wid )
    return;

  myWin.remove( id );

  QDockWidget* dock = windowDock( wid );
  if ( !dock )
    return;

  dock->setWidget( 0 );
  wid->setParent( 0 );
  wid->setVisible( false );
  delete dock;
}

void LightApp_Application::placeDockWindow( const int id, Qt::DockWidgetArea place )
{
  QDockWidget* dock = windowDock( dockWindow( id ) );
  if ( dock && desktop() ) {
    desktop()->addDockWidget( place, dock );
    QtxDockAction* a = qobject_cast<QtxDockAction*>( action( ViewWindowsId ) );
    if ( a ) a->update();
  }
}

/*!
  Gets window.
  \param flag - key for window
  Flag used how identificator of window in windows list.
*/
QWidget* LightApp_Application::getWindow( const int flag)
{
  QWidget* wid = dockWindow( flag );
  if ( !wid )
    insertDockWindow( flag, wid = createWindow( flag ) );

  QMap<int, int> winMap;
  currentWindows( winMap );
  if ( winMap.contains( flag ) )
    placeDockWindow( flag, (Qt::DockWidgetArea)winMap[flag] );

  return wid;
}

/*!
  \return Object Browser
*/
SUIT_DataBrowser* LightApp_Application::objectBrowser()
{
  return qobject_cast<SUIT_DataBrowser*>( dockWindow( WT_ObjectBrowser ) );
}

QtxInfoPanel* LightApp_Application::infoPanel()
{
  return qobject_cast<QtxInfoPanel *>( dockWindow( WT_InfoPanel ));
}

/*!
  \return Log Window
*/
LogWindow* LightApp_Application::logWindow()
{
  return qobject_cast<LogWindow*>( dockWindow( WT_LogWindow ) );
}

#ifndef DISABLE_PYCONSOLE
/*!
  This returns the python console integrated to the GUI. Depending
  when you request the python console, this function could return
  null. Then the optional parameter force (default to false) can be
  set to force the creation of the python console if it is not done
  already.
  \param force - if true, the pythonConsole is created if it does not exist yet
  \return Python Console
*/
PyConsole_Console* LightApp_Application::pythonConsole(const bool force)
{
  QWidget* wid = dockWindow( WT_PyConsole );
  if ( !wid && force==true) {
    wid = getWindow(WT_PyConsole);
  }
  return qobject_cast<PyConsole_Console*>( wid );
}
#endif

/*!
  Updates object browser and maybe data models
  \param updateModels - if it is true, then data models are updated
*/
void LightApp_Application::updateObjectBrowser( const bool updateModels )
{
  // update existing data models
  if ( updateModels )
  {
    const bool isAutoUpdate = objectBrowser() ? objectBrowser()->autoUpdate() : true;
    if ( objectBrowser() )
      objectBrowser()->setAutoUpdate( false );

    LightApp_Study* study = dynamic_cast<LightApp_Study*>(activeStudy());
    if ( study ) {
      CAM_Study::ModelList dm_list;
      study->dataModels( dm_list );
      QListIterator<CAM_DataModel*> it( dm_list );
      while ( it.hasNext() ) {
        CAM_DataModel* camDM = it.next();
        if ( camDM && camDM->inherits( "LightApp_DataModel" ) )
          ((LightApp_DataModel*)camDM)->update();
      }
    }

    if( objectBrowser() )
      objectBrowser()->setAutoUpdate( isAutoUpdate );
  }

  if ( objectBrowser() ) {
    objectBrowser()->updateGeometry();
    objectBrowser()->updateTree( 0, false );
  }
}

/*!
  \return preferences
*/
LightApp_Preferences* LightApp_Application::preferences() const
{
  return preferences( false );
}

/*!
  \return first view manager of some type
  \param vmType - type of view manager
  \param create - is it necessary to create view manager in case, when there is no manager of such type
*/
SUIT_ViewManager* LightApp_Application::getViewManager( const QString& vmType, const bool create )
{
  SUIT_ViewManager* aVM = viewManager( vmType );
  SUIT_ViewManager* anActiveVM = CAM_Application::activeViewManager();
  MESSAGE("vmType: " << vmType.toStdString() << " aVM: " << aVM << " anActiveVM: " << anActiveVM );
  if ( anActiveVM && anActiveVM->getType() == vmType )
    {
      MESSAGE("aVM = anActiveVM");
      aVM = anActiveVM;
    }

  bool keepDetached = property("keep_detached").toBool();
  if ( aVM && (!aVM->getDetached() || keepDetached) && create )
  {
    if ( !aVM->getActiveView() )
      {
        MESSAGE("aVM->createView()");
        aVM->createView();
      }
    else
      {
        MESSAGE("desktop()->setActiveWindow: " << aVM->getActiveView());
        desktop()->setActiveWindow( aVM->getActiveView() );
      }
  }
  else if ( create )
    {
      MESSAGE("aVM = createViewManager( vmType )");
      aVM = createViewManager( vmType );
    }

  return aVM;
}

/*!
  Creates view manager of some type
  \param vmType - type of view manager
*/
SUIT_ViewManager* LightApp_Application::createViewManager( const QString& vmType, bool detached )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();

  SUIT_ViewManager* viewMgr = 0;
#ifndef DISABLE_GLVIEWER
  if( vmType == GLViewer_Viewer::Type() )
  {
    viewMgr = new GLViewer_ViewManager( activeStudy(), desktop() );
    new LightApp_GLSelector( (GLViewer_Viewer2d*)viewMgr->getViewModel(), mySelMgr );
  }
#endif
#ifndef DISABLE_PLOT2DVIEWER
  if( vmType == Plot2d_Viewer::Type() )
  {
    viewMgr = new Plot2d_ViewManager( activeStudy(), desktop() );
    Plot2d_Viewer* vm;
#ifndef DISABLE_SALOMEOBJECT
    SPlot2d_Viewer* v = new SPlot2d_Viewer();
    vm = v;
    new LightApp_Plot2dSelector( v, mySelMgr );
#else
    vm = new Plot2d_Viewer();
#endif
    viewMgr->setViewModel( vm  );// custom view model, which extends SALOME_View interface
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( viewMgr->getActiveView() );
    if( wnd )
    {
      Plot2d_ViewFrame* frame = wnd->getViewFrame();
      frame->setBackgroundColor( resMgr->colorValue( "Plot2d", "Background", frame->backgroundColor() ) );
    }
  }
#endif
#ifndef DISABLE_QXGRAPHVIEWER
  if( vmType == QxScene_Viewer::Type() )
  {
    viewMgr = new QxScene_ViewManager( activeStudy(), desktop() );
    QxScene_Viewer* vm = new QxScene_Viewer();
    viewMgr->setViewModel( vm  );
    //QxScene_ViewWindow* wnd = dynamic_cast<QxScene_ViewWindow*>( viewMgr->getActiveView() );
  }
#endif
#ifndef DISABLE_GRAPHICSVIEW
  if( vmType == GraphicsView_Viewer::Type() )
  {
    viewMgr = new GraphicsView_ViewManager( activeStudy(), desktop() );
    new LightApp_GVSelector( (GraphicsView_Viewer*)viewMgr->getViewModel(), mySelMgr );
  }
#endif
#ifndef DISABLE_PVVIEWER
  if( vmType == PVViewer_Viewer::Type() )
  {
    if (( viewMgr = dynamic_cast<PVViewer_ViewManager*>( getViewManager( vmType, false )))) {
      viewMgr->getActiveView()->setFocus();
      return 0;
    } else {
      viewMgr = new PVViewer_ViewManager( activeStudy(), desktop() );
    }
  }
#endif
#ifndef DISABLE_PYVIEWER
  if( vmType == PyViewer_Viewer::Type() )
  {
    viewMgr = new PyViewer_ViewManager( activeStudy(), desktop() );
  }
#endif
#ifndef DISABLE_OCCVIEWER
  if( vmType == OCCViewer_Viewer::Type() )
  {
    viewMgr = new OCCViewer_ViewManager( activeStudy(), desktop() );
    OCCViewer_Viewer* vm;
#ifndef DISABLE_SALOMEOBJECT
    vm = new SOCC_Viewer();
#else
    vm = new OCCViewer_Viewer( true );
#endif
    vm->setBackground( OCCViewer_ViewFrame::TOP_LEFT,
                       resMgr->backgroundValue( "OCCViewer", "xz_background", vm->background(OCCViewer_ViewFrame::TOP_LEFT) ) );
    vm->setBackground( OCCViewer_ViewFrame::TOP_RIGHT,
                       resMgr->backgroundValue( "OCCViewer", "yz_background", vm->background(OCCViewer_ViewFrame::TOP_RIGHT) ) );
    vm->setBackground( OCCViewer_ViewFrame::BOTTOM_LEFT,
                       resMgr->backgroundValue( "OCCViewer", "xy_background", vm->background(OCCViewer_ViewFrame::BOTTOM_LEFT) ) );
    vm->setBackground( OCCViewer_ViewFrame::BOTTOM_RIGHT,
                       resMgr->backgroundValue( "OCCViewer", "background", vm->background(OCCViewer_ViewFrame::MAIN_VIEW) ) );

    vm->setTrihedronSize(  resMgr->doubleValue( "3DViewer", "trihedron_size", vm->trihedronSize() ),
                           resMgr->booleanValue( "3DViewer", "relative_size", vm->trihedronRelative() ));
    vm->setInteractionStyle( resMgr->integerValue( "3DViewer", "navigation_mode", vm->interactionStyle() ) );
    vm->setProjectionType( resMgr->integerValue( "OCCViewer", "projection_mode", vm->projectionType() ) );
    vm->setStereoType( resMgr->integerValue( "OCCViewer", "stereo_type", vm->stereoType() ) );
    vm->setAnaglyphFilter( resMgr->integerValue( "OCCViewer", "anaglyph_filter", vm->anaglyphFilter() ) );
    vm->setStereographicFocus( resMgr->integerValue( "OCCViewer", "focus_type", vm->stereographicFocusType() ),
                               resMgr->doubleValue( "OCCViewer", "focus_value", vm->stereographicFocusValue() ));
    vm->setInterocularDistance( resMgr->integerValue( "OCCViewer", "iod_type", vm->interocularDistanceType() ),
                                resMgr->doubleValue( "OCCViewer", "iod_value", vm->interocularDistanceValue() ));
    vm->setSelectionStyle((OCCViewer_ViewWindow::SelectionStyle) resMgr->integerValue( "OCCViewer", "adv_selection_mode", vm->selectionStyle() ) );

    vm->setReverseStereo( resMgr->booleanValue( "OCCViewer", "reverse_stereo", vm->isReverseStereo() ) );
    vm->setVSync( resMgr->booleanValue( "OCCViewer", "enable_vsync", vm->isVSync() ) );
    vm->setQuadBufferSupport( resMgr->booleanValue( "OCCViewer", "enable_quad_buffer_support", vm->isQuadBufferSupport() ) );
    vm->setZoomingStyle( resMgr->integerValue( "3DViewer", "zooming_mode", vm->zoomingStyle() ) );
    vm->enablePreselection( resMgr->booleanValue( "OCCViewer", "enable_preselection", vm->isPreselectionEnabled() ) );
    vm->enableSelection(    resMgr->booleanValue( "OCCViewer", "enable_selection",    vm->isSelectionEnabled() ) );
    vm->setClippingColor( resMgr->colorValue( "OCCViewer", "clipping_color", vm->clippingColor() ) );
    vm->setClippingTextureParams( resMgr->booleanValue( "OCCViewer", "clipping_use_default_texture", vm->isDefaultTextureUsed() ),
                                  resMgr->stringValue( "OCCViewer", "clipping_texture", vm->clippingTexture() ),
                                  resMgr->booleanValue( "OCCViewer", "clipping_modulate", vm->isTextureModulated() ),
                                  resMgr->doubleValue( "OCCViewer", "clipping_scale", vm->clippingTextureScale() ) );


    viewMgr->setViewModel( vm );// custom view model, which extends SALOME_View interface
    new LightApp_OCCSelector( (OCCViewer_Viewer*)viewMgr->getViewModel(), mySelMgr );
  }
#endif
#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
  if ( vmType == SVTK_Viewer::Type() )
#else
  if ( vmType == VTKViewer_Viewer::Type() )
#endif
  {
#ifndef DISABLE_SALOMEOBJECT
    viewMgr = new SVTK_ViewManager( activeStudy(), desktop() );
    SVTK_Viewer* vm = dynamic_cast<SVTK_Viewer*>( viewMgr->getViewModel() );
    if( vm )
    {
      vm->setProjectionMode( resMgr->integerValue( "VTKViewer", "projection_mode", vm->projectionMode() ) );
      vm->setStereoType( resMgr->integerValue( "VTKViewer", "stereo_type", vm->stereoType() ) );
      vm->setAnaglyphFilter( resMgr->integerValue( "VTKViewer", "anaglyph_filter", vm->anaglyphFilter() ) );
      vm->setQuadBufferSupport( resMgr->booleanValue( "VTKViewer", "enable_quad_buffer_support", vm->isQuadBufferSupport() ) );
      vm->setBackground( resMgr->backgroundValue( "VTKViewer", "background", vm->background() ) );
      vm->setTrihedronSize( resMgr->doubleValue( "3DViewer", "trihedron_size", vm->trihedronSize() ),
                            resMgr->booleanValue( "3DViewer", "relative_size", vm->trihedronRelative() ) );
      vm->setStaticTrihedronVisible( resMgr->booleanValue( "3DViewer", "show_static_trihedron", vm->isStaticTrihedronVisible() ) );
      vm->setInteractionStyle( resMgr->integerValue( "3DViewer", "navigation_mode", vm->interactionStyle() ) );
      vm->setZoomingStyle( resMgr->integerValue( "3DViewer", "zooming_mode", vm->zoomingStyle() ) );
      vm->setPreSelectionMode(resMgr->integerValue( "VTKViewer", "preselection", vm->preSelectionMode() ) );
      vm->enableSelection( resMgr->booleanValue( "VTKViewer", "enable_selection", vm->isSelectionEnabled() ) );
      vm->setIncrementalSpeed( resMgr->integerValue( "VTKViewer", "speed_value", vm->incrementalSpeed() ),
                               resMgr->integerValue( "VTKViewer", "speed_mode", vm->incrementalSpeedMode() ) );
      vm->setSpacemouseButtons( resMgr->integerValue( "VTKViewer", "spacemouse_func1_btn", vm->spacemouseBtn(1) ),
                                resMgr->integerValue( "VTKViewer", "spacemouse_func2_btn", vm->spacemouseBtn(2) ),
                                resMgr->integerValue( "VTKViewer", "spacemouse_func5_btn", vm->spacemouseBtn(3) ) );
      new LightApp_VTKSelector( vm, mySelMgr );
    }
#else
    viewMgr = new VTKViewer_ViewManager( activeStudy(), desktop() );
    VTKViewer_Viewer* vm = dynamic_cast<VTKViewer_Viewer*>( viewMgr->getViewModel() );
    if ( vm )
      vm->setBackground( resMgr->backgroundValue( "VTKViewer", "background", vm->background() ) );
#endif
  }
#endif

#ifndef DISABLE_PV3DVIEWER
# ifndef DISABLE_SALOMEOBJECT
  if ( vmType == SPV3D_ViewModel::Type() )
# else
  if ( vmType == PV3DViewer_ViewModel::Type() )
# endif
  {
    viewMgr = new SPV3D_ViewManager( activeStudy(), desktop() );
    SPV3D_ViewModel* vm = dynamic_cast<SPV3D_ViewModel*>( viewMgr->getViewModel() );
    if ( vm )
    {
      // vm->setBackground(...); //NYI
      // vm->...

      new LightApp_PV3DSelector( vm, mySelMgr );
    }
#else
    viewMgr = new PV3DViewer_ViewManager( activeStudy(), desktop() );
    PV3DViewer_ViewModel* vm = dynamic_cast<PV3DViewer_ViewModel*>( viewMgr->getViewModel() );
    if ( vm )
    {
      // vm->setBackground(...); //NYI
    }
#endif
  }

  if ( !viewMgr )
    return 0;

  viewMgr->setDetached(detached);
  addViewManager( viewMgr );
  SUIT_ViewWindow* viewWin = viewMgr->createViewWindow();

  if ( viewWin && desktop() ) {
    viewWin->resize( (int)( desktop()->width() * 0.6 ), (int)( desktop()->height() * 0.6 ) );
    viewWin->setDropDownButtons( resMgr->booleanValue( "viewers", "drop_down_buttons", true ) );
  }

  return viewMgr;
}

SUIT_ViewManager* LightApp_Application::createViewManager( const QString& vmType, QWidget* w )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();

  SUIT_ViewManager* vm = new SUIT_ViewManager( activeStudy(),
                                               desktop(),
                                               new LightApp_WgViewModel( vmType, w ) );
  vm->setTitle( QString( "%1: %M - viewer %V" ).arg( vmType ) );

  addViewManager( vm );
  SUIT_ViewWindow* vw = vm->createViewWindow();
  if ( vw && desktop() ) {
    vw->resize( (int)( desktop()->width() * 0.6 ), (int)( desktop()->height() * 0.6 ) );
    vw->setDropDownButtons( resMgr->booleanValue( "viewers", "drop_down_buttons", true ) );
  }

  if ( !vmType.isEmpty() && !myUserWmTypes.contains( vmType ) )
    myUserWmTypes << vmType;

  return vm;
}

SUIT_ViewManager* LightApp_Application::createViewManager( SUIT_ViewModel* theModel )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();

  SUIT_ViewManager* vm = new SUIT_ViewManager( activeStudy(),
                                               desktop(),
                                               theModel );

  QString vmType = vm->getType();

  vm->setTitle( QString( "%1: %M - viewer %V" ).arg( vmType ) );

  addViewManager( vm );
  SUIT_ViewWindow* vw = vm->createViewWindow();
  if ( vw && desktop() ) {
    vw->resize( (int)( desktop()->width() * 0.6 ), (int)( desktop()->height() * 0.6 ) );
    vw->setDropDownButtons( resMgr->booleanValue( "viewers", "drop_down_buttons", true ) );
  }

  if ( !vmType.isEmpty() && !myUserWmTypes.contains( vmType ) )
    myUserWmTypes << vmType;

  return vm;
}

/*!
  SLOT: Removes view manager from application
*/
void LightApp_Application::onCloseView( SUIT_ViewManager* theVM )
{
  removeViewManager( theVM );
}

/*!
  Protected SLOT: On study created.
  \param theStudy - just created study
*/
void LightApp_Application::onStudyCreated( SUIT_Study* theStudy )
{
  SUIT_DataObject* aRoot = 0;
  if ( theStudy && theStudy->root() )
  {
    aRoot = theStudy->root();
    //aRoot->setName( tr( "DATA_MODELS" ) );
  }

  getWindow( WT_ObjectBrowser );
  getWindow( WT_InfoPanel );

  loadDockWindowsState();

  if ( objectBrowser() )
    objectBrowser()->setRoot( aRoot );

  activateModule( defaultModule() );

  if ( objectBrowser() )
    objectBrowser()->openLevels();

#ifndef DISABLE_PYCONSOLE
  if( pythonConsole() )
    getPyInterp()->initStudy();
#endif
}

/*!
  Protected SLOT: On study opened.
  \param theStudy - just opened  study
*/
void LightApp_Application::onStudyOpened( SUIT_Study* theStudy )
{
  SUIT_DataObject* aRoot = 0;
  if ( theStudy && theStudy->root() )
  {
    aRoot = theStudy->root();
    //aRoot->dump();
  }

  getWindow( WT_ObjectBrowser );
  getWindow( WT_InfoPanel );

  loadDockWindowsState();

  if ( objectBrowser() )
    objectBrowser()->setRoot( aRoot );

  activateModule( defaultModule() );

  if ( objectBrowser() )
    objectBrowser()->openLevels();

#ifndef DISABLE_PYCONSOLE
  if( pythonConsole() )
    getPyInterp()->initStudy();
#endif

  emit studyOpened();
}

/*!Protected SLOT. On study saved.*/
void LightApp_Application::onStudySaved( SUIT_Study* s )
{
  QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );
  if ( mru && s )
    mru->insert( s->studyName() );

  emit studySaved();
}

/*!Protected SLOT. On study closed.*/
void LightApp_Application::onStudyClosed( SUIT_Study* /*s*/ )
{
  /*
  disconnect( this, SIGNAL( viewManagerRemoved( SUIT_ViewManager* ) ),
              this, SLOT( onViewManagerRemoved( SUIT_ViewManager* ) ) );
  */

  // stop auto-save timer
  myAutoSaveTimer->stop();

  // Bug 10396: clear selection
  mySelMgr->clearSelected();

  // Bug 12944: emit signal only after clear selection
  emit studyClosed();

  activateModule( "" );
}

/*!Protected SLOT.On desktop activated.*/
void LightApp_Application::onDesktopActivated()
{
  CAM_Application::onDesktopActivated();
  LightApp_Module* aModule = dynamic_cast<LightApp_Module*>(activeModule());
  if(aModule)
    aModule->studyActivated();
}

void LightApp_Application::studyOpened( SUIT_Study* s )
{
  CAM_Application::studyOpened( s );

  updateWindows();
  updateViewManagers();
}

void LightApp_Application::studySaved( SUIT_Study* s )
{
  CAM_Application::studyOpened( s );
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  if ( aResMgr && activeStudy() ) {
    int autoSaveInterval = aResMgr->integerValue( "Study", "auto_save_interval", 0 );
    if ( autoSaveInterval > 0 ) myAutoSaveTimer->start( autoSaveInterval*60000 );
  }
}

void LightApp_Application::studyCreated( SUIT_Study* s )
{
  CAM_Application::studyCreated( s );

  updateWindows();
  updateViewManagers();
}

/*!Gets file filter.
 *\retval QString "(*.hdf)"
 */
QString LightApp_Application::getFileFilter( bool /*open*/) const
{
  return "(*.hdf)";
}

/*!
  Shows file dialog and return user selected file name
*/
QString LightApp_Application::getFileName( bool open, const QString& initial, const QString& filters,
                                           const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  QStringList fls = filters.split( ";;", QString::SkipEmptyParts );
  return SUIT_FileDlg::getFileName( parent, initial, fls, caption, open, true );
}

/*! Gets directory*/
QString LightApp_Application::getDirectory( const QString& initial, const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  return SUIT_FileDlg::getExistingDirectory( parent, initial, caption, true );
}

/*! Get open file names*/
QStringList LightApp_Application::getOpenFileNames( const QString& initial, const QString& filters,
                                                    const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  QStringList fls = filters.split( ";;", QString::SkipEmptyParts );
  return SUIT_FileDlg::getOpenFileNames( parent, initial, fls, caption, true );
}

/*!Private SLOT. Update object browser.*/
void LightApp_Application::onRefresh()
{
  updateObjectBrowser( true );
}

/*!Private SLOT. Update actions after rename object.*/
void LightApp_Application::onRenamed()
{
  activeStudy()->Modified();
  updateActions();
}

// IMN 08.07.2015 : issue 002556: Some stereo outputs are affected by window position.
// To prevent reversion the window should be either aligned during movement and resize.
/*!Private SLOT. Update actions after rename object.*/
/*void LightApp_Application::onMoved()
{
  OCCViewer_ViewManager* viewMgr = 0;
  viewMgr = dynamic_cast<OCCViewer_ViewManager*>( getViewManager( OCCViewer_Viewer::Type(), false ) );
  if (viewMgr) {
    OCCViewer_ViewWindow* view = 0;
    view = dynamic_cast<OCCViewer_ViewWindow*>( viewMgr->getActiveView() );
    if (view) {
      view->getViewPort()->repaintViewAfterMove();
    }
  }
}
*/
/*!Private SLOT. Support drag-and-drop operation.*/
void LightApp_Application::onDropped( const QList<SUIT_DataObject*>& objects, SUIT_DataObject* parent, int row, Qt::DropAction action )
{
  LightApp_DataObject* parentObj = dynamic_cast<LightApp_DataObject*>( parent );
  if ( !parentObj )
    return;

  LightApp_Module* aModule = dynamic_cast<LightApp_Module*>( parentObj->module() );
  if ( aModule )
    aModule->dropObjects( objects, parentObj, row, action );
}

/*!Private SLOT. On preferences.*/
void LightApp_Application::onPreferences()
{
  showPreferences( activeModule() ? activeModule()->moduleName() : tr( "PREF_CATEGORY_SALOME" ) );
}

/*!Private SLOT. On preferences.*/
void LightApp_Application::showPreferences( const QString& path )
{
  showPreferences( QStringList() << path );
}

void LightApp_Application::showPreferences( const QStringList& path )
{
  QApplication::setOverrideCursor( Qt::WaitCursor );

  LightApp_PreferencesDlg* prefDlg = new LightApp_PreferencesDlg( preferences( true ), desktop());

  QApplication::restoreOverrideCursor();

  if ( !prefDlg )
    return;

  preferences()->activateItem( path );

  if ( ( prefDlg->exec() == QDialog::Accepted || prefDlg->isSaved() ) &&  resourceMgr() )
  {
    if ( desktop() )
      resourceMgr()->setValue( "desktop", "geometry", desktop()->storeGeometry() );
    resourceMgr()->save();

    // Update shortcuts
    shortcutMgr()->updateShortcuts();
  }

  delete prefDlg;
}

/*!Protected SLOT. On preferences changed.*/
void LightApp_Application::onPreferenceChanged( QString& modName, QString& section, QString& param )
{
  LightApp_Module* sMod = 0;
  CAM_Module* mod = module( modName );
  if ( mod && mod->inherits( "LightApp_Module" ) )
    sMod = (LightApp_Module*)mod;

  if ( sMod )
    sMod->preferencesChanged( section, param );
  else
    preferencesChanged( section, param );
  // emit signal to allow additional preferences changing processing
  emit preferenceChanged( modName, section, param );
}

/*!Remove all windows from study.*/
void LightApp_Application::beforeCloseDoc( SUIT_Study* s )
{
  saveDockWindowsState();

  if ( SUIT_DataBrowser* ob = objectBrowser() )
    ob->setModel(0);

  CAM_Application::beforeCloseDoc( s );
}

/*!Update actions.*/
void LightApp_Application::updateActions()
{
  updateCommandsStatus();
}

/*!
  Creates new study
*/
SUIT_Study* LightApp_Application::createNewStudy()
{
  LightApp_Study* aStudy = new LightApp_Study( this );

  // Set up processing of major study-related events
  connect( aStudy, SIGNAL( created( SUIT_Study* ) ), this, SLOT( onStudyCreated( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( opened ( SUIT_Study* ) ), this, SLOT( onStudyOpened ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( saved  ( SUIT_Study* ) ), this, SLOT( onStudySaved  ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( closed ( SUIT_Study* ) ), this, SLOT( onStudyClosed ( SUIT_Study* ) ) );

  return aStudy;
}

/*!
  Creates window by flag.
  \param flag - identificator of window type
*/
QWidget* LightApp_Application::createWindow( const int flag )
{
  QWidget* wid = 0;

  SUIT_ResourceMgr* resMgr = resourceMgr();

  if ( flag == WT_ObjectBrowser )
  {
    SUIT_DataBrowser* ob = new SUIT_DataBrowser( new LightApp_DataObject(), desktop() );
    ob->setObjectName( "objectBrowser" );
    ob->setSortMenuEnabled( true );
    ob->setAutoUpdate( true );
    if ( resMgr->hasValue( "ObjectBrowser", "auto_hide_search_tool" ) )
      ob->searchTool()->enableAutoHide( resMgr->booleanValue( "ObjectBrowser", "auto_hide_search_tool" ) );

    //ob->setAutoOpenLevel( 1 ); // commented by ASV as a fix to bug IPAL10107
    ob->setWindowTitle( tr( "OBJECT_BROWSER" ) );
    connect( ob, SIGNAL( requestUpdate() ), this, SLOT( onRefresh() ) );

    QString EntryCol = QObject::tr( "ENTRY_COLUMN" );
    SUIT_AbstractModel* treeModel = dynamic_cast<SUIT_AbstractModel*>( ob->model() );
    treeModel->setSearcher( this );
    treeModel->registerColumn( 0, EntryCol, LightApp_DataObject::EntryId );
    treeModel->setAppropriate( EntryCol, Qtx::Toggled );

    // Mantis issue 0020136: Drag&Drop in OB
    SUIT_ProxyModel* proxyModel = dynamic_cast<SUIT_ProxyModel*>(treeModel);
    if ( proxyModel ) {
      connect( proxyModel, SIGNAL( dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ),
               this,       SLOT( onDropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ) );
      connect( proxyModel, SIGNAL( renamed( SUIT_DataObject* ) ),
               this,       SLOT( onRenamed( ) ) );

    }

    // temporary commented
    /*
    OB_ListView* ob_list = dynamic_cast<OB_ListView*>( const_cast<QListView*>( ob->listView() ) );
    if( ob_list )
      ob_list->setColumnMaxWidth( 0, desktop()->width()/4 );
    */

    // Create OBSelector
    new LightApp_OBSelector( ob, mySelMgr );
    ob->treeView()->header()->setSectionResizeMode(SUIT_DataObject::VisibilityId, QHeaderView::Fixed);
    ob->treeView()->header()->moveSection(SUIT_DataObject::NameId,SUIT_DataObject::VisibilityId);
    ob->treeView()->setColumnWidth(SUIT_DataObject::VisibilityId, VISIBILITY_COLUMN_WIDTH);
    ob->setProperty( "shortcut", QKeySequence( "Alt+Shift+O" ) );
    wid = ob;
    ob->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
  else if ( flag == WT_InfoPanel)
  {
    QtxInfoPanel* ipanel = new QtxInfoPanel( desktop() );
    ipanel->setObjectName( "infoPanel" );
    ipanel->setWindowTitle( tr( "INFO_PANEL" ) );
    wid = ipanel;
  }
#ifndef DISABLE_PYCONSOLE
  else  if ( flag == WT_PyConsole )
  {
    PyConsole_Console* pyCons = new PyConsole_Console( desktop(), new LightApp_PyEditor( getPyInterp() ) );
    pyCons->setObjectName( "pythonConsole" );
    pyCons->setWindowTitle( tr( "PYTHON_CONSOLE" ) );
    pyCons->setFont( resMgr->fontValue( "PyConsole", "font" ) );
    pyCons->setIsShowBanner( resMgr->booleanValue( "PyConsole", "show_banner", true ) );
    pyCons->setAutoCompletion( resMgr->booleanValue( "PyConsole", "auto_completion", true ) );
    pyCons->setProperty( "shortcut", QKeySequence( "Alt+Shift+P" ) );
    wid = pyCons;
  }
#endif
  else if ( flag == WT_LogWindow )
  {
    LogWindow* logWin = new LogWindow( desktop() );
    logWin->handleQtMessages( true );
    logWin->setObjectName( "logWindow" );
    logWin->setWindowTitle( tr( "LOG_WINDOW" ) );
    logWin->setProperty( "shortcut", QKeySequence( "Alt+Shift+L" ) );
    wid = logWin;
    logWin->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
  return wid;
}

/*!
  \return default windows( Object Browser, Python Console )
  Adds to map \a aMap.
 */
void LightApp_Application::defaultWindows( QMap<int, int>& aMap ) const
{
#ifndef DISABLE_PYCONSOLE
  aMap.insert( WT_PyConsole, Qt::BottomDockWidgetArea );
#endif
  if ( activeStudy() ) {
    aMap.insert( WT_ObjectBrowser, Qt::LeftDockWidgetArea );
    aMap.insert( WT_InfoPanel, Qt::RightDockWidgetArea );
    //  aMap.insert( WT_LogWindow, Qt::DockBottom );
  }
}

/*!Default view managers*/
void LightApp_Application::defaultViewManagers( QStringList& ) const
{
  /*!Do nothing.*/
}

/*!
  \return preferences.
  Create preferences, if \a crt = true.
*/
LightApp_Preferences* LightApp_Application::preferences( const bool crt ) const
{
  if ( myPrefs )
    return myPrefs;

  LightApp_Application* that = (LightApp_Application*)this;

  bool toCreate = !_prefs_ && crt;
  if ( toCreate )
  {
    _prefs_ = new LightApp_Preferences( resourceMgr() );
    that->createPreferences( _prefs_ );
    qAddPostRoutine( LightAppCleanUpAppResources );
  }

  that->myPrefs = _prefs_;

  connect( myPrefs, SIGNAL( preferenceChanged( QString&, QString&, QString& ) ),
           this, SLOT( onPreferenceChanged( QString&, QString&, QString& ) ), Qt::UniqueConnection );
  connect( myPrefs, SIGNAL( resetToDefaults() ),
           this, SIGNAL( preferenceResetToDefaults() ), Qt::UniqueConnection );

  if ( !crt )
    return myPrefs;

  QList<SUIT_Application*> appList = SUIT_Session::session()->applications();
  for ( QList<SUIT_Application*>::iterator appIt = appList.begin(); appIt != appList.end(); ++appIt )
  {
    LightApp_Application* app = ::qobject_cast<LightApp_Application*>( *appIt );
    if ( !app )
      continue;

    // all modules available in current session
    QStringList names;
    app->modules( names, false );

    // step 1: iterate through list of all available modules
    // and add empty preferences page
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
    {
      if ( !_prefs_->hasModule( *it ) ) // prevent possible duplications
      {
        int modId = _prefs_->addPreference( *it ); // add empty page
        _prefs_->setItemIcon( modId, moduleIcon( *it, 20 ) ); // scale icon to 20x20 pix
      }
    }

    // step 2: iterate through list of all loaded modules
    // and initialize their preferences
    ModuleList loadedModules;
    app->modules( loadedModules );
    QListIterator<CAM_Module*> itr( loadedModules );
    while ( itr.hasNext() )
    {
      LightApp_Module* module = 0;
      CAM_Module* m = itr.next();
      if ( m->inherits( "LightApp_Module" ) )
        module = (LightApp_Module*)m;

      if ( module && !_prefs_->hasModule( module->moduleName() ) )
      {
        _prefs_->addPreference( module->moduleName() ); // add page (for sure, had to be done at step 1)
        module->createPreferences();                    // initialize preferences
        that->emptyPreferences( module->moduleName() ); // show dummy page if module does not export any preferences
      }
    }
  }
  _prefs_->setItemProperty( "info", tr( "PREFERENCES_NOT_LOADED" ) ); // dummy page for modules which are not loaded yet

  return myPrefs;
}

/*!
  Adds new module to application
*/
void LightApp_Application::moduleAdded( CAM_Module* mod )
{
  CAM_Application::moduleAdded( mod );

  LightApp_Module* lightMod = 0;
  if ( mod && mod->inherits( "LightApp_Module" ) )
    lightMod = (LightApp_Module*)mod;

  if ( myPrefs && lightMod && !myPrefs->hasModule( lightMod->moduleName() ))
  {
    myPrefs->addPreference( mod->moduleName() );
    lightMod->createPreferences();
    emptyPreferences( mod->moduleName() );
  }
}

void LightApp_Application::moduleDeactivated( CAM_Module* /*mod*/ )
{
  if ( infoPanel() )
    infoPanel()->clear();
}

void LightApp_Application::emptyPreferences( const QString& modName )
{
  QtxPreferenceItem* item = myPrefs->findItem( modName, true );
  if ( !item || !item->isEmpty() )
    return;

  QtxPagePrefFrameItem* frm = new QtxPagePrefFrameItem( item->title(), item->parentItem() );
  frm->setIcon( item->icon() );
  frm->setStretch( false );
  item->parentItem()->insertItem( frm, item );
  new QtxPagePrefLabelItem( Qt::AlignCenter, tr( "PREFERENCES_NOT_SUPPORTED" ).arg( modName ), frm );
  delete item;
}

/*!
  Create preferences
*/
void LightApp_Application::createPreferences( LightApp_Preferences* pref )
{
  if ( !pref )
    return;

  QStringList     aValuesList;
  QList<QVariant> anIndicesList;
  QIntList        idList;
  QIntList        txtList;

  // . Top-level "SALOME" preferences group <<start>>
  int salomeCat = pref->addPreference( tr( "PREF_CATEGORY_SALOME" ) );
  pref->setItemIcon( salomeCat, Qtx::scaleIcon( resourceMgr()->loadPixmap( "LightApp", tr( "APP_DEFAULT_ICO" ), false ), 20 ) );

  // .. "General" preferences tab <<start>>
  int genTab = pref->addPreference( tr( "PREF_TAB_GENERAL" ), salomeCat );

  // ... "Language" group <<start>>
  int langGroup = pref->addPreference( tr( "PREF_GROUP_LANGUAGE" ), genTab );
  pref->setItemProperty( "columns", 2, langGroup );
  // .... -> application language
  int curLang = pref->addPreference( tr( "PREF_CURRENT_LANGUAGE" ), langGroup,
                                          LightApp_Preferences::Selector, "language", "language" );
  QStringList aLangs = SUIT_Session::session()->resourceMgr()->stringValue( "language", "languages", "en" ).split( "," );
  QList<QVariant> aIcons;
  QList<QVariant> aNumbers;
  QStringList aTitles;
  foreach ( QString aLang, aLangs ) {
    aIcons << QPixmap( QString( ":/images/%1" ).arg( aLang ) );
    aNumbers << aLang;
    aTitles << langToName( aLang );
  }
  pref->setItemProperty( "strings", aTitles, curLang );
  pref->setItemProperty( "ids",     aNumbers, curLang );
  pref->setItemProperty( "icons",   aIcons, curLang );
  pref->setItemProperty( "restart",  true, curLang );

  int curLocale = pref->addPreference( tr( "PREF_CURRENT_LOCALE" ), langGroup,
                                          LightApp_Preferences::Bool, "language", "locale" );
  pref->setItemProperty( "restart",  true, curLocale );
  // ... "Language" group <<end>>

  // ... "Look and feel" group <<start>>
  int lookGroup = pref->addPreference( tr( "PREF_GROUP_LOOK_AND_FEEL" ), genTab );
  pref->setItemProperty( "columns", 2, lookGroup );
  // .... -> show splash-screen
  pref->addPreference( tr( "PREF_SHOW_SPLASH" ), lookGroup, LightApp_Preferences::Bool, "launch", "splash" );
  // .... -> opaque resize
  pref->addPreference( tr( "PREF_OPAQUE_RESIZE" ), lookGroup, LightApp_Preferences::Bool, "desktop", "opaque_resize" );
  // .... -> drop-down buttons
  pref->addPreference( tr( "PREF_DROP_DOWN_BUTTONS" ), lookGroup, LightApp_Preferences::Bool, "viewers", "drop_down_buttons" );
  // .... -> Notification timeout
  int delay = pref->addPreference( tr( "PREF_NOTIFY_TIMEOUT" ), lookGroup, LightApp_Preferences::IntSpin, "notification", "timeout" );
  pref->setItemProperty( "special", tr("PREF_NOTIFY_TIMEOUT_NONE"), delay );
  pref->setItemProperty( "min", 0, delay );
  pref->setItemProperty( "max", 100, delay );
  pref->setItemProperty( "suffix", " sec", delay );
  // ... "Look and feel" group <<end>>

  // ... "Study properties" group <<start>>
  int studyGroup = pref->addPreference( tr( "PREF_GROUP_STUDY" ), genTab );
  pref->setItemProperty( "columns", 2, studyGroup );
  // .... -> multi-file save
  pref->addPreference( tr( "PREF_MULTI_FILE" ), studyGroup, LightApp_Preferences::Bool, "Study", "multi_file" );
  // .... -> ascii save mode
  pref->addPreference( tr( "PREF_ASCII_FILE" ), studyGroup, LightApp_Preferences::Bool, "Study", "ascii_file" );
  // .... -> store windows geometry
  pref->addPreference( tr( "PREF_LOAD_LIGHT" ), studyGroup, LightApp_Preferences::Bool, "Study", "autoload_light_modules" );
  pref->addPreference( tr( "PREF_STORE_POS" ),  studyGroup, LightApp_Preferences::Bool, "Study", "store_positions" );
  pref->addPreference( "", studyGroup, LightApp_Preferences::Space );
  pref->addPreference( tr( "PREF_STORE_TOOL_POS" ),  studyGroup, LightApp_Preferences::Bool, "Study", "store_tool_positions" );
  // .... -> auto-save
  int autoSaveInterval = pref->addPreference( tr( "PREF_AUTO_SAVE" ),  studyGroup,
                                              LightApp_Preferences::IntSpin, "Study", "auto_save_interval" );
  pref->setItemProperty( "min",        0, autoSaveInterval );
  pref->setItemProperty( "max",     1440, autoSaveInterval );
  pref->setItemProperty( "special", tr( "PREF_AUTO_SAVE_DISABLED" ), autoSaveInterval );
  // ... "Study properties" group <<end>>

  // ... "Help browser" group <<start>>
#ifndef DISABLE_QTXWEBBROWSER
  int extgroup = pref->addPreference( tr( "PREF_GROUP_EXT_BROWSER" ), genTab, LightApp_Preferences::Auto, "ExternalBrowser", "use_external_browser");
#else
  int extgroup = pref->addPreference( tr( "PREF_GROUP_EXT_BROWSER" ), genTab );
#endif

#ifdef WIN32
  QString platform = "winapplication";
#else
  QString platform = "application";
#endif
  // .... -> browser application
  int apppref = pref->addPreference( tr( "PREF_APP" ), extgroup, LightApp_Preferences::File, "ExternalBrowser", platform );
  pref->setItemProperty( "mode", Qtx::PT_OpenFile, apppref );
  // .... -> browser parameters
  pref->addPreference( tr( "PREF_PARAM" ), extgroup, LightApp_Preferences::String, "ExternalBrowser", "parameters" );
  // ... "Help browser" group <<end>>

  // ... "Python console properties" group <<start>>
  int pythonConsoleGroup = pref->addPreference( tr( "PREF_GROUP_PY_CONSOLE" ), genTab );
  pref->setItemProperty( "columns", 2, pythonConsoleGroup );
  // .... -> font
  pref->addPreference( tr( "PREF_FONT" ), pythonConsoleGroup, LightApp_Preferences::Font, "PyConsole", "font" );
  // .... -> show banner
  pref->addPreference( tr( "PREF_SHOW_BANNER" ), pythonConsoleGroup, LightApp_Preferences::Bool, "PyConsole", "show_banner" );
  // .... -> auto-completion
  pref->addPreference( tr( "PREF_AUTO_COMPLETION" ), pythonConsoleGroup, LightApp_Preferences::Bool, "PyConsole", "auto_completion" );
  // ... "Python console properties" group <<end>>

  // ... "MRU" preferences group <<start>>
  int mruGroup = pref->addPreference( tr( "PREF_GROUP_MRU" ), genTab, LightApp_Preferences::Auto, "MRU", "show_mru" );
  pref->setItemProperty( "columns", 4, mruGroup );
  // number of MRU items
  int mruVisCount = pref->addPreference( tr( "PREF_MRU_VISIBLE_COUNT" ), mruGroup, LightApp_Preferences::IntSpin,
                                         "MRU", "visible_count" );
  pref->setItemProperty( "min", 0,   mruVisCount );
  pref->setItemProperty( "max", 100, mruVisCount );
  // MRU links type
  int mruLinkType = pref->addPreference( tr( "PREF_MRU_LINK_TYPE" ), mruGroup, LightApp_Preferences::Selector,
                                         "MRU", "link_type" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_MRU_LINK_AUTO") << tr("PREF_MRU_LINK_SHORT") << tr("PREF_MRU_LINK_FULL");
  anIndicesList << 0                        << 1                         << 2                       ;
  pref->setItemProperty( "strings", aValuesList,   mruLinkType );
  pref->setItemProperty( "indexes", anIndicesList, mruLinkType );
  // ... "MRU" preferences group <<end>>

  // ... "Full-screen" group <<start>>
  int fullScreenGroup = pref->addPreference( tr( "PREF_GROUP_FULL_SCREEN" ), genTab );
  pref->setItemProperty( "columns", 2, fullScreenGroup );
  // .... -> automatic hiding toolbars
  pref->addPreference( tr( "PREF_FULL_SCREEN_AUTO" ), fullScreenGroup,
                       LightApp_Preferences::Bool, "OCCViewer", "automatic_hiding" );
  // ... "Full-screen" group <<end>>

  // .. "General" preferences tab <<end>>

  // .. "3D viewer" group <<start>>
  int Viewer3DGroup = pref->addPreference( tr( "PREF_GROUP_3DVIEWER" ), salomeCat );
  // ... -> navigation mode
  int vtkStyleMode = pref->addPreference( tr( "PREF_NAVIGATION" ), Viewer3DGroup,
                                          LightApp_Preferences::Selector, "3DViewer", "navigation_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_STANDARD_STYLE") << tr("PREF_KEYFREE_STYLE");
  anIndicesList << 0                         << 1;
  pref->setItemProperty( "strings", aValuesList,   vtkStyleMode );
  pref->setItemProperty( "indexes", anIndicesList, vtkStyleMode );
  // ... -> zooming mode
  int occZoomingStyleMode = pref->addPreference( tr( "PREF_ZOOMING" ), Viewer3DGroup,
                                                 LightApp_Preferences::Selector, "3DViewer", "zooming_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ZOOMING_AT_CENTER") << tr("PREF_ZOOMING_AT_CURSOR");
  anIndicesList << 0                            << 1;
  pref->setItemProperty( "strings", aValuesList,   occZoomingStyleMode );
  pref->setItemProperty( "indexes", anIndicesList, occZoomingStyleMode );
  // ... "Trihedron" group <<start>>
  int occTriGroup = pref->addPreference( tr( "PREF_TRIHEDRON" ), Viewer3DGroup );
  pref->setItemProperty( "columns", 2, occTriGroup );
  // .... -> trihedron size
  int occTS = pref->addPreference( tr( "PREF_TRIHEDRON_SIZE" ), occTriGroup,
                                   LightApp_Preferences::DblSpin, "3DViewer", "trihedron_size" );
  pref->setItemProperty( "min", 1.0E-06, occTS );
  pref->setItemProperty( "max", 1000, occTS );
  // .... -> relative size of trihedron
  pref->addPreference( tr( "PREF_RELATIVE_SIZE" ), occTriGroup, LightApp_Preferences::Bool, "3DViewer", "relative_size" );
  // .... -> show static trihedron
  pref->addPreference( tr( "PREF_SHOW_STATIC_TRIHEDRON" ), occTriGroup, LightApp_Preferences::Bool, "3DViewer", "show_static_trihedron" );
  // ... "Trihedron" group <<end>>
  // .. "3D viewer" group <<end>>

  QString formats;
  int bgId;
#ifndef DISABLE_OCCVIEWER
  // .. "OCC viewer" group <<start>>
  int occGroup = pref->addPreference( tr( "PREF_GROUP_OCCVIEWER" ), salomeCat );

  // .... -> Projection mode
  int occProjMode = pref->addPreference( tr( "PREF_PROJECTION_MODE" ), occGroup,
                                         LightApp_Preferences::Selector, "OCCViewer", "projection_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ORTHOGRAPHIC") << tr("PREF_PERSPECTIVE");
  anIndicesList << 0                       << 1;
  pref->setItemProperty( "strings", aValuesList,   occProjMode );
  pref->setItemProperty( "indexes", anIndicesList, occProjMode );

  // .... -> Stereo group
  int stereoGroup = pref->addPreference( tr( "PREF_GROUP_STEREO" ), occGroup);
  pref->setItemProperty( "columns", 2, stereoGroup );
  // .... -> Stereo type
  int stereoType = pref->addPreference( tr( "PREF_STEREO_TYPE" ), stereoGroup,
                                            LightApp_Preferences::Selector, "OCCViewer", "stereo_type" );
  aValuesList.clear();
  anIndicesList.clear();
  idList.clear();
  OCCViewer_Viewer::stereoData( aValuesList, idList);
  foreach( int gid, idList ) anIndicesList << gid;
  pref->setItemProperty( "strings", aValuesList,   stereoType );
  pref->setItemProperty( "indexes", anIndicesList, stereoType );

  // .... -> Anaglyph filter
  int anaglyphFilter = pref->addPreference( tr( "PREF_ANAGLYPH_FILTER" ), stereoGroup,
                                            LightApp_Preferences::Selector, "OCCViewer", "anaglyph_filter" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ANAGLYPH_RED_CYAN") << tr("PREF_ANAGLYPH_YELLOW_BLUE") << tr("PREF_ANAGLYPH_GREEN_MAGENTA");
  anIndicesList << 0                            << 1                               << 2;

  pref->setItemProperty( "strings", aValuesList,   anaglyphFilter );
  pref->setItemProperty( "indexes", anIndicesList, anaglyphFilter );

  // .... -> Convergence distance type
  int occFocusType = pref->addPreference( tr( "PREF_FOCUS_TYPE" ), stereoGroup,
                                           LightApp_Preferences::Selector, "OCCViewer", "focus_type" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ABSOLUTE") << tr("PREF_RELATIVE");
  anIndicesList << 0                   << 1;
  pref->setItemProperty( "strings", aValuesList,   occFocusType );
  pref->setItemProperty( "indexes", anIndicesList, occFocusType );

  // .... -> Stereographic focus value
  int focusValue = pref->addPreference( tr( "PREF_FOCUS_VALUE" ), stereoGroup,
               LightApp_Preferences::DblSpin, "OCCViewer", "focus_value" );
  pref->setItemProperty( "precision", 3, focusValue );
  pref->setItemProperty( "min", 1.0E-03, focusValue );
  pref->setItemProperty( "max", 1.0E03, focusValue );
  pref->setItemProperty( "step", 0.05, focusValue );

  // .... -> IOD type
  int occIODType = pref->addPreference( tr( "PREF_IOD_TYPE" ), stereoGroup,
                                           LightApp_Preferences::Selector, "OCCViewer", "iod_type" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ABSOLUTE") << tr("PREF_RELATIVE");
  anIndicesList << 0                   << 1;
  pref->setItemProperty( "strings", aValuesList,   occIODType );
  pref->setItemProperty( "indexes", anIndicesList, occIODType );

  // .... -> Interocular distance (IOD) value
  int IODValue = pref->addPreference( tr( "PREF_IOD_VALUE" ), stereoGroup,
                                      LightApp_Preferences::DblSpin, "OCCViewer", "iod_value" );
  pref->setItemProperty( "precision", 3, IODValue );
  pref->setItemProperty( "min", 1.0E-03, IODValue );
  pref->setItemProperty( "max", 1.0E03, IODValue );
  pref->setItemProperty( "step", 0.05, IODValue );

  // .... -> Reverse stereo
  pref->addPreference( tr( "PREF_REVERSE_STEREO" ), stereoGroup,
                       LightApp_Preferences::Bool, "OCCViewer", "reverse_stereo" );
  // .... -> Enable V-Sync
  pref->addPreference( tr( "PREF_ENABLE_VSYNC" ), stereoGroup,
                       LightApp_Preferences::Bool, "OCCViewer", "enable_vsync" );
  // .... -> Enable quad-buffer support
  pref->addPreference( tr( "PREF_ENABLE_QUAD_BUFFER_SUPPORT" ), stereoGroup,
                       LightApp_Preferences::Bool, "OCCViewer", "enable_quad_buffer_support" );

  // ... "Background" group <<start>>
  int bgGroup = pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), occGroup );
  //  pref->setItemProperty( "columns", 2, bgGroup );
  aValuesList.clear();
  anIndicesList.clear();
  txtList.clear();
  idList.clear();
  formats = OCCViewer_Viewer::backgroundData( aValuesList, idList, txtList );
  foreach( int gid, idList ) anIndicesList << gid;
  // .... -> 3D viewer background
  bgId = pref->addPreference( tr( "PREF_3DVIEWER_BACKGROUND" ), bgGroup,
                                  LightApp_Preferences::Background, "OCCViewer", "background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
  pref->setItemProperty( "image_formats", formats, bgId );
  // .... -> XZ viewer background
  bgId = pref->addPreference( tr( "PREF_XZVIEWER_BACKGROUND" ), bgGroup,
                              LightApp_Preferences::Background, "OCCViewer", "xz_background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
  pref->setItemProperty( "image_formats", formats, bgId );
  // .... -> YZ viewer background
  bgId = pref->addPreference( tr( "PREF_YZVIEWER_BACKGROUND" ), bgGroup,
                              LightApp_Preferences::Background, "OCCViewer", "yz_background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
  pref->setItemProperty( "image_formats", formats, bgId );
  // .... -> XY viewer background
  bgId = pref->addPreference( tr( "PREF_XYVIEWER_BACKGROUND" ), bgGroup,
                              LightApp_Preferences::Background, "OCCViewer", "xy_background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
  pref->setItemProperty( "image_formats", formats, bgId );
  // ... "Background" group <<end>>


  // ... "Selection" group <<start>>
  int occSelectionGroup = pref->addPreference( tr( "PREF_GROUP_SELECTION" ), occGroup );
  pref->setItemProperty( "columns", 3, occSelectionGroup );
  // .... -> enable preselection
  pref->addPreference( tr( "PREF_ENABLE_PRESELECTION" ), occSelectionGroup,
                       LightApp_Preferences::Bool, "OCCViewer", "enable_preselection" );
  // .... -> enable selection
  pref->addPreference( tr( "PREF_ENABLE_SELECTION" ), occSelectionGroup,
                       LightApp_Preferences::Bool, "OCCViewer", "enable_selection" );
  // .... -> selection style
  int aSeleStyle = pref->addPreference( tr( "PREF_SELECTION_STYLE" ), occSelectionGroup,
                       LightApp_Preferences::Selector, "OCCViewer", "adv_selection_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_POLYGON_SELECTION") << tr("PREF_CIRCLE_SELECTION");
  anIndicesList << 0 << 1;
  pref->setItemProperty( "strings", aValuesList, aSeleStyle);
  pref->setItemProperty( "indexes", anIndicesList, aSeleStyle);
  // ... "Selection" group <<end>>

  // ... "Clipping" group <<start>>
  int occClippingGroup = pref->addPreference( tr( "PREF_GROUP_CLIPPING" ), occGroup );
  // .... -> clipping color
  pref->addPreference( tr( "PREF_CLIPPING_COLOR" ), occClippingGroup,
                       LightApp_Preferences::Color, "OCCViewer", "clipping_color" );
  int texturePref = pref->addPreference( "", occClippingGroup, LightApp_Preferences::Frame );
  pref->setItemProperty( "columns", 2, texturePref );
  // .... -> use default texture
  pref->addPreference( tr( "PREF_CLIPPING_DEFAULT_TEXTURE" ), texturePref,
               LightApp_Preferences::Bool, "OCCViewer", "clipping_use_default_texture" );
  // .... -> clipping texture
  int filePref = pref->addPreference( tr( "PREF_CLIPPING_TEXTURE" ), texturePref,
               LightApp_Preferences::File, "OCCViewer", "clipping_texture" );
  pref->setItemProperty( "path_filter", tr( "OCC_TEXTURE_FILES" ), filePref );
  // .... -> modulate
  pref->addPreference( tr( "PREF_CLIPPING_MODULATE" ), texturePref,
               LightApp_Preferences::Bool, "OCCViewer", "clipping_modulate" );
  // .... -> scale factor
  int scaleFactor = pref->addPreference( tr( "PREF_CLIPPING_SCALE" ), texturePref,
               LightApp_Preferences::DblSpin, "OCCViewer", "clipping_scale" );
  pref->setItemProperty( "precision", 3, scaleFactor );
  pref->setItemProperty( "min", 1.0E-03, scaleFactor );
  pref->setItemProperty( "max", 1.0E03, scaleFactor );
  pref->setItemProperty( "step", 0.1, scaleFactor );
  // ... "Clipping" group <<end>>

  // ... "Ray tracing" group <<start>>
  int occRayTracingGroup = pref->addPreference( tr( "PREF_GROUP_RAY_TRACING" ), occGroup );
  int rtPref = pref->addPreference( "", occRayTracingGroup, LightApp_Preferences::Frame );
  pref->setItemProperty( "columns", 2, rtPref );
  // .... -> depth
  int rt_depth = pref->addPreference( tr( "PREF_RAY_TRACING_DEPTH" ), rtPref,
               LightApp_Preferences::IntSpin, "OCCViewer", "rt_depth" );
  pref->setItemProperty( "min", 1, rt_depth );
  pref->setItemProperty( "max", 10, rt_depth );
  pref->setItemProperty( "step", 1, rt_depth );
  pref->addPreference( "", rtPref, LightApp_Preferences::Frame );
  // .... -> specular reflections
  pref->addPreference( tr( "PREF_RAY_TRACING_REFLECTION" ), rtPref,
               LightApp_Preferences::Bool, "OCCViewer", "rt_reflection" );
  // .... -> adaptive anti-aliasing
  pref->addPreference( tr( "PREF_RAY_TRACING_ANTIALIASING" ), rtPref,
               LightApp_Preferences::Bool, "OCCViewer", "rt_antialiasing" );
  // .... -> shadows rendering
  pref->addPreference( tr( "PREF_RAY_TRACING_SHADOW" ), rtPref,
               LightApp_Preferences::Bool, "OCCViewer", "rt_shadow" );
  // .... -> transparent shadow
  pref->addPreference( tr( "PREF_RAY_TRACING_TRANS_SHADOW" ), rtPref,
               LightApp_Preferences::Bool, "OCCViewer", "rt_trans_shadow" );
  // ... "Ray tracing" group <<end>>

  // ... "Light source" group <<start>>
  int occLightGroup = pref->addPreference( tr( "PREF_GROUP_LIGHT" ), occGroup );
  // .... -> light color
  pref->addPreference( tr( "PREF_LIGHT_COLOR" ), occLightGroup,
               LightApp_Preferences::Color, "OCCViewer", "light_color" );
  int directionPref = pref->addPreference( "", occLightGroup, LightApp_Preferences::Frame );
  pref->setItemProperty( "columns", 3, directionPref );
  // .... -> light direction (dx component)
  int light_dx = pref->addPreference( tr( "Dx" ), directionPref,
               LightApp_Preferences::DblSpin, "OCCViewer", "light_dx" );
  pref->setItemProperty( "precision", 2, light_dx );
  pref->setItemProperty( "min", -1.0E03, light_dx );
  pref->setItemProperty( "max", 1.0E03, light_dx );
  pref->setItemProperty( "step", 0.1, light_dx );
  // .... -> light direction (dy component)
  int light_dy = pref->addPreference( tr( "Dy" ), directionPref,
               LightApp_Preferences::DblSpin, "OCCViewer", "light_dy" );
  pref->setItemProperty( "precision", 2, light_dy );
  pref->setItemProperty( "min", -1.0E03, light_dy );
  pref->setItemProperty( "max", 1.0E03, light_dy );
  pref->setItemProperty( "step", 0.1, light_dy );
  // .... -> light direction (dz component)
  int light_dz = pref->addPreference( tr( "Dz" ), directionPref,
               LightApp_Preferences::DblSpin, "OCCViewer", "light_dz" );
  pref->setItemProperty( "precision", 2, light_dz );
  pref->setItemProperty( "min", -1.0E03, light_dz );
  pref->setItemProperty( "max", 1.0E03, light_dz );
  pref->setItemProperty( "step", 0.1, light_dz );
  // ... "Light source" group <<end>>

  // ... "View cube" group <<start>>
  int occViewCubeGroup = pref->addPreference( tr( "PREF_GROUP_VIEWCUBE" ), occGroup );
  pref->setItemProperty( "columns", 2, occViewCubeGroup );
  // .... -> show view cube on viewer start
  pref->addPreference( tr( "PREF_VIEWCUBE_SHOW" ), occViewCubeGroup,
               LightApp_Preferences::Bool, "OCCViewer", "viewcube_show" );
  // .... -> view cube duration of animation (sec)
  int viewcube_dur = pref->addPreference( tr( "PREF_VIEWCUBE_DURATION" ), occViewCubeGroup,
               LightApp_Preferences::DblSpin, "OCCViewer", "viewcube_duration" );
  pref->setItemProperty( "min", 0.1, viewcube_dur );
  pref->setItemProperty( "max", 10.0, viewcube_dur );
  pref->setItemProperty( "step", 0.1, viewcube_dur );
  // .... -> show view cube axes
  pref->addPreference( tr( "PREF_VIEWCUBE_AXES" ), occViewCubeGroup,
               LightApp_Preferences::Bool, "OCCViewer", "viewcube_axes" );
  // ... "View cube" group <<end>>

  // ... "View cube default (OCCT) attributes" group <<start>>
  int occViewCubeAttrsGroup = pref->addPreference( tr( "PREF_VIEWCUBE_CUSTOM" ), occGroup,
               LightApp_Preferences::Auto, "OCCViewer", "viewcube_custom" );
  pref->setItemProperty( "columns", 2, occViewCubeAttrsGroup );
  // .... -> box color
  pref->addPreference( tr( "PREF_VIEWCUBE_COLOR" ), occViewCubeAttrsGroup,
               LightApp_Preferences::Color, "OCCViewer", "viewcube_color" );
  // .... -> view cube size
  int viewcube_size = pref->addPreference( tr( "PREF_VIEWCUBE_SIZE" ), occViewCubeAttrsGroup,
               LightApp_Preferences::DblSpin, "OCCViewer", "viewcube_size" );
  pref->setItemProperty( "min",  30.0, viewcube_size );
  pref->setItemProperty( "max", 100.0, viewcube_size );
  pref->setItemProperty( "step", 10.0, viewcube_size );
  // .... -> text color
  pref->addPreference( tr( "PREF_VIEWCUBE_TEXTCOLOR" ), occViewCubeAttrsGroup,
               LightApp_Preferences::Color, "OCCViewer", "viewcube_text_color" );
  // ... "View cube" group <<end>>

  // ... -> empty frame (for layout) <<start>>
  int occGen = pref->addPreference( "", occGroup, LightApp_Preferences::Frame );
  pref->setItemProperty( "margin",  0, occGen );
  pref->setItemProperty( "columns", 2, occGen );
  // ... -> empty frame (for layout) <<end>>

  // .. "OCC viewer" group <<end>>
#endif

#ifndef DISABLE_VTKVIEWER
  // .. "VTK viewer" group <<start>>
  int vtkGroup = pref->addPreference( tr( "PREF_GROUP_VTKVIEWER" ), salomeCat ); //viewTab

  // ... -> empty frame (for layout) <<start>>
  int vtkGen = pref->addPreference( "", vtkGroup, LightApp_Preferences::Frame );
  //pref->setItemProperty( "columns", 2, vtkGen );
  // .... -> projection mode
  int vtkProjMode = pref->addPreference( tr( "PREF_PROJECTION_MODE" ), vtkGen,
                                         LightApp_Preferences::Selector, "VTKViewer", "projection_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ORTHOGRAPHIC") << tr("PREF_PERSPECTIVE");
  anIndicesList << 0                       << 1;
  pref->setItemProperty( "strings", aValuesList,   vtkProjMode );
  pref->setItemProperty( "indexes", anIndicesList, vtkProjMode );

  // .... -> Stereo group
  int vtkStereoGroup = pref->addPreference( tr( "PREF_GROUP_STEREO" ), vtkGroup);
  pref->setItemProperty( "columns", 2, vtkStereoGroup );
  // .... -> Stereo type
  int vtkStereoType = pref->addPreference( tr( "PREF_STEREO_TYPE" ), vtkStereoGroup,
                                           LightApp_Preferences::Selector, "VTKViewer", "stereo_type" );
  aValuesList.clear();
  anIndicesList.clear();
  idList.clear();
  SVTK_Viewer::stereoData( aValuesList, idList);
  foreach( int gid, idList ) anIndicesList << gid;
  pref->setItemProperty( "strings", aValuesList,   vtkStereoType );
  pref->setItemProperty( "indexes", anIndicesList, vtkStereoType );
  // .... -> Anaglyph filter
  int vtkAnaglyphFilter = pref->addPreference( tr( "PREF_ANAGLYPH_FILTER" ), vtkStereoGroup,
                                               LightApp_Preferences::Selector, "VTKViewer", "anaglyph_filter" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ANAGLYPH_RED_CYAN") << tr("PREF_ANAGLYPH_YELLOW_BLUE") << tr("PREF_ANAGLYPH_GREEN_MAGENTA");
  anIndicesList << 0                            << 1                               << 2;

  pref->setItemProperty( "strings", aValuesList,   vtkAnaglyphFilter );
  pref->setItemProperty( "indexes", anIndicesList, vtkAnaglyphFilter );

  // .... -> Enable quad-buffer support
  pref->addPreference( tr( "PREF_ENABLE_QUAD_BUFFER_SUPPORT" ), vtkStereoGroup,
                       LightApp_Preferences::Bool, "VTKViewer", "enable_quad_buffer_support" );

  // .... -> background
  aValuesList.clear();
  anIndicesList.clear();
  txtList.clear();
  idList.clear();
#ifndef DISABLE_SALOMEOBJECT
  formats = SVTK_Viewer::backgroundData( aValuesList, idList, txtList );
#endif
  foreach( int gid, idList ) anIndicesList << gid;
  bgId = pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), vtkGen,
                              LightApp_Preferences::Background, "VTKViewer", "background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
#ifndef DISABLE_SALOMEOBJECT
  pref->setItemProperty( "image_formats", formats, bgId );
#endif
  // .... -> speed increment
  int vtkSpeed = pref->addPreference( tr( "PREF_INCREMENTAL_SPEED" ), vtkGen,
                                      LightApp_Preferences::IntSpin, "VTKViewer", "speed_value" );
  pref->setItemProperty( "min", 1, vtkSpeed );
  pref->setItemProperty( "max", 1000, vtkSpeed );
  // .... -> speed mode
  int vtkSpeedMode = pref->addPreference( tr( "PREF_INCREMENTAL_SPEED_MODE" ), vtkGen,
                                          LightApp_Preferences::Selector, "VTKViewer", "speed_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ARITHMETIC") << tr("PREF_GEOMETRICAL");
  anIndicesList << 0                     << 1;
  pref->setItemProperty( "strings", aValuesList,   vtkSpeedMode );
  pref->setItemProperty( "indexes", anIndicesList, vtkSpeedMode );

  // ... "Selection" group <<start>>
  int vtkSelectionGroup = pref->addPreference( tr( "PREF_GROUP_SELECTION" ), vtkGroup );
  pref->setItemProperty( "columns", 2, vtkSelectionGroup );
  // .... -> preselection
  int vtkPreselection = pref->addPreference( tr( "PREF_PRESELECTION" ),  vtkSelectionGroup,
                                             LightApp_Preferences::Selector, "VTKViewer", "preselection" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_PRESELECTION_STANDARD") << tr("PREF_PRESELECTION_DYNAMIC") << tr("PREF_PRESELECTION_DISABLED");
  anIndicesList << 0 << 1 << 2;
  pref->setItemProperty( "strings", aValuesList,   vtkPreselection );
  pref->setItemProperty( "indexes", anIndicesList, vtkPreselection );
  // .... -> enable selection
  pref->addPreference( tr( "PREF_ENABLE_SELECTION" ), vtkSelectionGroup, LightApp_Preferences::Bool, "VTKViewer", "enable_selection" );
  // ... "Selection" group <<end>>

  // ... -> empty frame (for layout) <<end>>

  // ... space mouse sub-group <<start>>
  int vtkSM = pref->addPreference( tr( "PREF_FRAME_SPACEMOUSE" ), vtkGroup, LightApp_Preferences::GroupBox );
  //pref->setItemProperty( "columns", 2, vtkSM );
  // .... -> decrease speed increment
  int spacemousePref1 = pref->addPreference( tr( "PREF_SPACEMOUSE_FUNC_1" ), vtkSM,
                                             LightApp_Preferences::Selector, "VTKViewer",
                                             "spacemouse_func1_btn" );
  // .... -> increase speed increment
  int spacemousePref2 = pref->addPreference( tr( "PREF_SPACEMOUSE_FUNC_2" ), vtkSM,
                                             LightApp_Preferences::Selector, "VTKViewer",
                                             "spacemouse_func2_btn" );
  // .... -> dominant / combined switch
  int spacemousePref3 = pref->addPreference( tr( "PREF_SPACEMOUSE_FUNC_3" ), vtkSM,
                                             LightApp_Preferences::Selector, "VTKViewer",
                                             "spacemouse_func5_btn" ); //
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList << tr( "PREF_SPACEMOUSE_BTN_1" )  << tr( "PREF_SPACEMOUSE_BTN_2" ) << tr( "PREF_SPACEMOUSE_BTN_3" );
  aValuesList << tr( "PREF_SPACEMOUSE_BTN_4" )  << tr( "PREF_SPACEMOUSE_BTN_5" ) << tr( "PREF_SPACEMOUSE_BTN_6" );
  aValuesList << tr( "PREF_SPACEMOUSE_BTN_7" )  << tr( "PREF_SPACEMOUSE_BTN_8" ) << tr( "PREF_SPACEMOUSE_BTN_*" );
  aValuesList << tr( "PREF_SPACEMOUSE_BTN_10" ) << tr( "PREF_SPACEMOUSE_BTN_11" );
  anIndicesList << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11;
  pref->setItemProperty( "strings", aValuesList,   spacemousePref1 );
  pref->setItemProperty( "indexes", anIndicesList, spacemousePref1 );
  pref->setItemProperty( "strings", aValuesList,   spacemousePref2 );
  pref->setItemProperty( "indexes", anIndicesList, spacemousePref2 );
  pref->setItemProperty( "strings", aValuesList,   spacemousePref3 );
  pref->setItemProperty( "indexes", anIndicesList, spacemousePref3 );
  // ... space mouse sub-group <<end>>

  // ... avi recording sub-group <<start>>
  int vtkRec = pref->addPreference( tr( "PREF_FRAME_RECORDING" ), vtkGroup, LightApp_Preferences::GroupBox );
  pref->setItemProperty( "columns", 2, vtkRec );
  // .... -> recording mode
  int modePref = pref->addPreference( tr( "PREF_RECORDING_MODE" ), vtkRec,
                                      LightApp_Preferences::Selector, "VTKViewer", "recorder_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr( "PREF_SKIPPED_FRAMES" ) << tr( "PREF_ALL_DISLPAYED_FRAMES" );
  anIndicesList << 0                           << 1;
  pref->setItemProperty( "strings", aValuesList,   modePref );
  pref->setItemProperty( "indexes", anIndicesList, modePref );
  // .... -> fps
  int fpsPref = pref->addPreference( tr( "PREF_FPS" ), vtkRec,
                                     LightApp_Preferences::DblSpin, "VTKViewer", "recorder_fps" );
  pref->setItemProperty( "min", 0.1, fpsPref );
  pref->setItemProperty( "max", 100, fpsPref );
  // .... -> quality
  int qualityPref = pref->addPreference( tr( "PREF_QUALITY" ), vtkRec,
                                         LightApp_Preferences::IntSpin, "VTKViewer", "recorder_quality" );
  pref->setItemProperty( "min", 1, qualityPref );
  pref->setItemProperty( "max", 100, qualityPref );
  // .... -> progressive mode
  pref->addPreference( tr( "PREF_PROGRESSIVE" ), vtkRec,
                       LightApp_Preferences::Bool, "VTKViewer", "recorder_progressive" );
  // ... avi recording sub-group <<end>>

  // ... group names sub-group <<start>>
  int vtkGN = pref->addPreference( tr( "PREF_FRAME_GROUP_NAMES" ), vtkGroup,
                                   LightApp_Preferences::GroupBox, "VTKViewer", "show_group_names" );
  pref->setItemProperty( "columns", 2, vtkGN );
  // .... -> text color
  pref->addPreference( tr(  "PREF_GROUP_NAMES_TEXT_COLOR" ), vtkGN,
                       LightApp_Preferences::Color, "VTKViewer", "group_names_text_color" );
  // .... -> transparency
  int transPref = pref->addPreference( tr( "PREF_GROUP_NAMES_TRANSPARENCY" ), vtkGN,
                                       LightApp_Preferences::DblSpin, "VTKViewer", "group_names_transparency" );
  pref->setItemProperty( "min", 0.0, transPref );
  pref->setItemProperty( "max", 1.0, transPref );
  pref->setItemProperty( "step", 0.1, transPref );
  // ... -> group names sub-group <<end>>
  // .. "VTK viewer" group <<end>>
#endif

  // .. "Plot2d viewer" group <<start>>
  int plot2dGroup = pref->addPreference( tr( "PREF_GROUP_PLOT2DVIEWER" ), salomeCat ); //viewTab
  //pref->setItemProperty( "columns", 2, plot2dGroup );

  // ... -> background
  pref->addPreference( tr( "PREF_VIEWER_BACKGROUND_COLOR" ), plot2dGroup,
                       LightApp_Preferences::Color, "Plot2d", "Background" );
  // ... -> selection color
  pref->addPreference( tr( "PREF_VIEWER_SELECTION" ), plot2dGroup,
                       LightApp_Preferences::Color, "Plot2d", "SelectionColor" );

  // ... "Viewer" group <<start>>
  int plot2dViewerGroup = pref->addPreference( tr( "PREF_GROUP_VIEWER" ), plot2dGroup );

  // .... -> curve type
  int curveType = pref->addPreference( tr( "PREF_CURVE_TYPE" ), plot2dViewerGroup,
                                       LightApp_Preferences::Selector, "Plot2d", "CurveType" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_POINTS") << tr("PREF_LINES") << tr("PREF_SPLINE");
  anIndicesList << 0                 << 1                << 2                ;
  pref->setItemProperty( "strings", aValuesList,   curveType );
  pref->setItemProperty( "indexes", anIndicesList, curveType );
  // .... -> marker size
  int markerSize = pref->addPreference( tr( "PREF_MARKER_SIZE" ), plot2dViewerGroup,
                                        LightApp_Preferences::IntSpin, "Plot2d", "MarkerSize" );
  pref->setItemProperty( "min", 0, markerSize );
  pref->setItemProperty( "max", 100, markerSize );
  // .... -> horizontal scaling mode
  int horScale = pref->addPreference( tr( "PREF_HOR_AXIS_SCALE" ), plot2dViewerGroup,
                                      LightApp_Preferences::Selector, "Plot2d", "HorScaleMode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_LINEAR") << tr("PREF_LOGARITHMIC");
  anIndicesList << 0                 << 1                     ;
  pref->setItemProperty( "strings", aValuesList,   horScale );
  pref->setItemProperty( "indexes", anIndicesList, horScale );
  // .... -> vertical scaling mode
  int verScale = pref->addPreference( tr( "PREF_VERT_AXIS_SCALE" ), plot2dViewerGroup,
                                      LightApp_Preferences::Selector, "Plot2d", "VerScaleMode" );
  pref->setItemProperty( "strings", aValuesList,   verScale );
  pref->setItemProperty( "indexes", anIndicesList, verScale );

  // .... -> errors/deviation colot
  pref->addPreference( tr( "PREF_DEVIATION_COLOR" ), plot2dViewerGroup,
                       LightApp_Preferences::Color, "Plot2d", "DeviationMarkerColor" );
  // .... -> deviation markers line size
  int deviationMarkerLw = pref->addPreference( tr( "PREF_DEVIATION_MARKER_LW" ), plot2dViewerGroup,
                                        LightApp_Preferences::IntSpin, "Plot2d", "DeviationMarkerLineWidth" );
  pref->setItemProperty( "min", 1, deviationMarkerLw );
  pref->setItemProperty( "max", 5, deviationMarkerLw );
  // .... -> deviation markers tick mark size
  int deviationMarkerTs = pref->addPreference( tr( "PREF_DEVIATION_MARKER_TS" ), plot2dViewerGroup,
                                        LightApp_Preferences::IntSpin, "Plot2d", "DeviationMarkerTickSize" );
  pref->setItemProperty( "min", 1, deviationMarkerTs );
  pref->setItemProperty( "max", 5, deviationMarkerTs );
  // .... "Viewer" group <<end>>

  // ... "Legend" group <<start>>
  int plot2dLegendGroup = pref->addPreference( tr( "PREF_GROUP_LEGEND" ), plot2dGroup );

  // .... -> show legend
  pref->addPreference( tr( "PREF_SHOW_LEGEND" ), plot2dLegendGroup,
                       LightApp_Preferences::Bool, "Plot2d", "ShowLegend" );
  // .... -> legend position
  int legendPosition = pref->addPreference( tr( "PREF_LEGEND_POSITION" ), plot2dLegendGroup,
                                            LightApp_Preferences::Selector, "Plot2d", "LegendPos" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_LEFT") << tr("PREF_RIGHT") << tr("PREF_TOP") << tr("PREF_BOTTOM");
  anIndicesList << 0               << 1                << 2              << 3                ;
  pref->setItemProperty( "strings", aValuesList,   legendPosition );
  pref->setItemProperty( "indexes", anIndicesList, legendPosition );
  // .... -> Symbol type
  int legendSymbolType = pref->addPreference( tr( "PREF_LEGEND_SYMBOL_TYPE" ), plot2dLegendGroup,
                                            LightApp_Preferences::Selector, "Plot2d", "LegendSymbolType" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_MARKER_ON_LINE") << tr("PREF_MARKER_ABOVE_LINE");
  anIndicesList << 0                            << 1                        ;
  pref->setItemProperty( "strings", aValuesList,   legendSymbolType );
  pref->setItemProperty( "indexes", anIndicesList, legendSymbolType );
  // .... -> legend font
  pref->addPreference( tr( "PREF_LEGEND_FONT" ), plot2dLegendGroup, LightApp_Preferences::Font, "Plot2d", "LegendFont" );
  // ... -> font color
  pref->addPreference( tr( "PREF_FONT_COLOR" ), plot2dLegendGroup, LightApp_Preferences::Color, "Plot2d", "LegendFontColor" );
  // ... -> selection font color
  pref->addPreference( tr( "PREF_SELECTED_FONT_COLOR" ), plot2dLegendGroup, LightApp_Preferences::Color, "Plot2d", "SelectedLegendFontColor" );
  // ... "Legend" group <<end>>

  // .. "Plot2d viewer" group <<end>>

  // .. "PyViewer" preferences tab <<start>>
  int pyeditTab = pref->addPreference( tr( "PREF_TAB_PYEDITOR" ), salomeCat );
  // ... "Font settings" group <<start>>
  int pyFontGroup = pref->addPreference( tr( "PREF_GROUP_PY_FONT" ), pyeditTab );
  int pyFont = pref->addPreference( tr( "PREF_PY_FONT" ), pyFontGroup,
                                    LightApp_Preferences::Font, "PyEditor", "Font" );
  pref->setItemProperty( "features", QtxFontEdit::Family | QtxFontEdit::Size | QtxFontEdit::UserSize, pyFont );
  // ... "Font settings" group <<end>>
  // ... "Display settings" group <<start>>
  int pyDispGroup = pref->addPreference( tr( "PREF_GROUP_PY_DISPLAY" ), pyeditTab );
  pref->setItemProperty( "columns", 2, pyDispGroup );
  // ... -> current line highlight
  pref->addPreference( tr( "PREF_PY_CURRLINE_HIGHLIGHT" ), pyDispGroup,
    LightApp_Preferences::Bool, "PyEditor", "HighlightCurrentLine" );
  // ... -> text wrapping
  pref->addPreference( tr( "PREF_PY_TEXT_WRAP" ), pyDispGroup,
    LightApp_Preferences::Bool, "PyEditor", "TextWrapping" );
  // ... -> center cursor on scroll
  pref->addPreference( tr( "PREF_PY_CURSON_ON_SCROLL" ), pyDispGroup,
    LightApp_Preferences::Bool, "PyEditor", "CenterCursorOnScroll" );
  // ... -> line numbers area
  pref->addPreference( tr( "PREF_PY_LINE_NUMBS_AREA" ), pyDispGroup,
    LightApp_Preferences::Bool, "PyEditor", "LineNumberArea" );
  // ... "Display settings" group <<end>>

  // ... "Editor settings" group <<start>>
  int pyEditGroup = pref->addPreference( tr( "PREF_GROUP_PY_EDITOR" ), pyeditTab );
  // ... -> navigation mode
  int pyCompletion = pref->addPreference( tr( "PREF_PY_COMPLETION_MODE" ), pyEditGroup,
                                          LightApp_Preferences::Selector, "PyEditor", "CompletionPolicy" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_PY_NONE") << tr("PREF_PY_AUTO") << tr("PREF_PY_MANUAL") << tr("PREF_PY_ALWAYS");
  anIndicesList << 0                  << 1                  << 2                    << 3                   ;
  pref->setItemProperty( "strings", aValuesList, pyCompletion );
  pref->setItemProperty( "indexes", anIndicesList, pyCompletion );
  // ... "Editor settings" group <<end>>

  // ... "Tab settings" group <<start>>
  int pyTabGroup = pref->addPreference( tr( "PREF_GROUP_PY_TAB" ), pyeditTab );
  pref->setItemProperty( "columns", 2, pyTabGroup );
  // ... -> tab whitespaces
  pref->addPreference( tr( "PREF_PY_TAB_WHITESPACES" ), pyTabGroup,
    LightApp_Preferences::Bool, "PyEditor", "TabSpaceVisible" );
  // ... -> tab size
  pref->addPreference( tr( "PREF_PY_TAB_SIZE" ), pyTabGroup,
    LightApp_Preferences::IntSpin, "PyEditor", "TabSize" );
  // ... "Tab settings" group <<end>>
  // ... "Vertical edge settings" group <<start>>
  int pyVertEdgeGroup = pref->addPreference( tr( "PREF_GROUP_VERT_EDGE" ), pyeditTab );
  pref->setItemProperty( "columns", 2, pyVertEdgeGroup );
  // ... -> vertical edge
  pref->addPreference( tr( "PREF_PY_VERT_EDGE" ), pyVertEdgeGroup,
    LightApp_Preferences::Bool, "PyEditor", "VerticalEdge" );
  // ... -> number of columns
  pref->addPreference( tr( "PREF_PY_NUM_COLUMNS" ), pyVertEdgeGroup,
    LightApp_Preferences::IntSpin, "PyEditor", "NumberColumns" );
  // ... "Vertical edge settings" group <<end>>
  // .. "PyEditor" preferences tab <<end>>

  // .. "Directories" preferences tab <<start>>
  int dirTab = pref->addPreference( tr( "PREF_TAB_DIRECTORIES" ), salomeCat );
  // ... --> quick directories list
  int dirGroup = pref->addPreference( tr( "PREF_GROUP_DIRECTORIES" ), dirTab );
  pref->addPreference( tr( "" ), dirGroup,
                       LightApp_Preferences::DirList, "FileDlg", "QuickDirList" );
  // .. "Directories" preferences tab <<end>>

  // .. "Object browser" preferences tab <<start>>
  int obTab = pref->addPreference( tr( "PREF_TAB_OBJBROWSER" ), salomeCat );

  // ... "Search tool" group <<start>>
  int stGroup = pref->addPreference( tr( "PREF_OBJ_BROWSER_SEARCH_TOOL" ), obTab );
  // .... --> auto-hide
  pref->addPreference( tr( "PREF_AUTO_HIDE_SEARCH_TOOL" ), stGroup, LightApp_Preferences::Bool,
                       "ObjectBrowser", "auto_hide_search_tool" );
  // ... "Search tool" group <<end>>

  // ... "Object browser settings" group <<start>>
  int objSetGroup = pref->addPreference( tr( "PREF_GROUP_LOOK_AND_FEEL" ), obTab );
  pref->setItemProperty( "columns", 2, objSetGroup );
  // .... -> auto size first column
  pref->addPreference( tr( "PREF_AUTO_SIZE_FIRST" ), objSetGroup, LightApp_Preferences::Bool,
                       "ObjectBrowser", "auto_size_first" );
  // .... -> auto size other columns
  pref->addPreference( tr( "PREF_AUTO_SIZE" ), objSetGroup, LightApp_Preferences::Bool,
                       "ObjectBrowser", "auto_size" );
  // .... -> resize columns on expand item
  pref->addPreference( tr( "PREF_RESIZE_ON_EXPAND_ITEM" ), objSetGroup, LightApp_Preferences::Bool,
                       "ObjectBrowser", "resize_on_expand_item" );
  // .... -> browse to published object
  int browsePublished = pref->addPreference( tr( "PREF_BROWSE_TO_THE_PUBLISHED_OBJECT" ), objSetGroup, LightApp_Preferences::Selector,
                                             "ObjectBrowser", "browse_published_object" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList << tr( "PREF_BROWSE_NEVER" ) << tr( "PREF_BROWSE_AFTER_APPLY_AND_CLOSE_ONLY" ) << tr( "PREF_BROWSE_ALWAYS" );
  anIndicesList << BP_Never << BP_ApplyAndClose << BP_Always;
  pref->setItemProperty( "strings", aValuesList,   browsePublished );
  pref->setItemProperty( "indexes", anIndicesList, browsePublished );
  // ... "Object browser settings" group <<end>>
  // .. "Object browser" preferences tab <<end>>

  // .. "Shortcuts" preferences tab <<start>>
  int shortcutTab = pref->addPreference( tr( "PREF_TAB_SHORTCUTS" ), salomeCat );
  // ... "Shortcuts settings" group <<start>>
  int shortcutGroup = pref->addPreference( tr( "PREF_GROUP_SHORTCUTS" ), shortcutTab );
  pref->addPreference( tr( "" ), shortcutGroup,
                       LightApp_Preferences::ShortcutTree, "shortcuts" );
  // ... "Shortcuts settings" group <<end>>
  // .. "Shortcuts" preferences tab <<end>>
  // . Top-level "SALOME" preferences group <<end>>

  pref->retrieve();
}

/*!
  Changes appearance of application according to changed preferences
  \param sec - section
  \param param - name of changed parameter
*/
void LightApp_Application::preferencesChanged( const QString& sec, const QString& param )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();
  if ( !resMgr )
    return;

  if ( sec == "viewers" && param == "drop_down_buttons" )
  {
    ViewManagerList vmlist = viewManagers();
    foreach( SUIT_ViewManager* vm, vmlist )
    {
      QVector<SUIT_ViewWindow*> vwlist = vm->getViews();
      foreach( SUIT_ViewWindow* vw, vwlist )
        if ( vw ) vw->setDropDownButtons( resMgr->booleanValue( "viewers", "drop_down_buttons", true ) );
    }
  }

  if ( sec == QString( "3DViewer" ) && (param == QString( "trihedron_size" ) || param == QString( "relative_size" )))
  {
    double sz = resMgr->doubleValue( sec, "trihedron_size", -1 );
    bool relative = resMgr->booleanValue( sec, "relative_size", true );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_OCCVIEWER
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> itOCC( lst );
    while ( itOCC.hasNext() && sz >= 0 )
    {
      SUIT_ViewModel* vm = itOCC.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setTrihedronSize( sz, relative );
      occVM->getAISContext()->UpdateCurrentViewer();
    }
#endif
#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> itVTK( lst );
    while ( itVTK.hasNext() && sz >= 0 )
    {
      SUIT_ViewModel* vm = itVTK.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM )
      {
        vtkVM->setTrihedronSize( sz, relative );
        vtkVM->Repaint();
      }
    }
#endif
#endif
  }

  if ( sec == QString( "3DViewer" ) && param == QString( "show_static_trihedron" ) )
  {
    bool isVisible = resMgr->booleanValue( "3DViewer", "show_static_trihedron", true );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_OCCVIEWER
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> itOCC( lst );
    while ( itOCC.hasNext() )
    {
      SUIT_ViewModel* vm = itOCC.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = dynamic_cast<OCCViewer_Viewer*>( vm );
      if( occVM )
      {
        occVM->setStaticTrihedronDisplayed( isVisible );
      }
    }
#endif
#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> itVTK( lst );
    while ( itVTK.hasNext() )
    {
      SUIT_ViewModel* vm = itVTK.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM )
      {
        vtkVM->setStaticTrihedronVisible( isVisible );
        vtkVM->Repaint();
      }
    }
#endif
#endif
  }

  if ( sec == QString( "3DViewer" ) && param == QString( "navigation_mode" ) )
  {
    int mode = resMgr->integerValue( "3DViewer", "navigation_mode", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_OCCVIEWER
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> itOCC( lst );
    while ( itOCC.hasNext() )
    {
      SUIT_ViewModel* vm = itOCC.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setInteractionStyle( mode );
    }
#endif
#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> itVTK( lst );
    while ( itVTK.hasNext() )
    {
      SUIT_ViewModel* vm = itVTK.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setInteractionStyle( mode );
    }
#endif
#endif
  }

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "enable_preselection" ) )
  {
    bool isToEnablePreselection = resMgr->booleanValue( "OCCViewer", "enable_preselection", true );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->enablePreselection( isToEnablePreselection );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "enable_selection" ) )
  {
    bool isToEnableSelection = resMgr->booleanValue( "OCCViewer", "enable_selection", true );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->enableSelection( isToEnableSelection );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "clipping_color" ) )
  {
    QColor aColor = resMgr->colorValue( "OCCViewer", "clipping_color", QColor( 50, 50, 50 ) );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setClippingColor( aColor );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && ( param == QString( "clipping_use_default_texture" ) ||
                                          param == QString( "clipping_texture" ) ||
                                          param == QString( "clipping_modulate" ) ||
                                          param == QString( "clipping_scale" ) ) )
  {
    bool isDefaultTextureUsed = resMgr->booleanValue( "OCCViewer", "clipping_use_default_texture" );
    QString aTexture = resMgr->stringValue( "OCCViewer", "clipping_texture" );
    bool isModulated = resMgr->booleanValue( "OCCViewer", "clipping_modulate" );
    double aScale = resMgr->doubleValue( "OCCViewer", "clipping_scale" );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setClippingTextureParams( isDefaultTextureUsed, aTexture, isModulated, aScale );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "projection_mode" ) )
  {
    int mode = resMgr->integerValue( "OCCViewer", "projection_mode", 0 );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setProjectionType( mode );
    }
  }
#endif


#ifndef DISABLE_OCCVIEWER
  if (sec == QString("OCCViewer") && param == QString("adv_selection_mode"))
  {
    int mode = resMgr->integerValue("OCCViewer", "adv_selection_mode", 0);
    QList<SUIT_ViewManager*> lst;
    viewManagers(OCCViewer_Viewer::Type(), lst);
    QListIterator<SUIT_ViewManager*> it(lst);
    while (it.hasNext())
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if (!vm || !vm->inherits("OCCViewer_Viewer"))
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setSelectionStyle((OCCViewer_ViewWindow::SelectionStyle)mode);
    }
  }
#endif


#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "stereo_type" ) )
  {
    int mode = resMgr->integerValue( "OCCViewer", "stereo_type", 0 );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setStereoType( mode );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "anaglyph_filter" ) )
  {
    int mode = resMgr->integerValue( "OCCViewer", "anaglyph_filter", 0 );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setAnaglyphFilter( mode );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && ( param == QString( "focus_type" ) ||
                                          param == QString( "focus_value" ) ) )
  {
    int aType = resMgr->integerValue( "OCCViewer", "focus_type" );
    double aValue = resMgr->doubleValue( "OCCViewer", "focus_value" );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setStereographicFocus( aType, aValue );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && ( param == QString( "iod_type" ) ||
                                          param == QString( "iod_value" ) ) )
  {
    int aType = resMgr->integerValue( "OCCViewer", "iod_type" );
    double aValue = resMgr->doubleValue( "OCCViewer", "iod_value" );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setInterocularDistance( aType, aValue );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "reverse_stereo" ) )
  {
    bool reverse = resMgr->booleanValue( "OCCViewer", "reverse_stereo", false );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setReverseStereo( reverse );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "enable_vsync" ) )
  {
    bool enable = resMgr->booleanValue( "OCCViewer", "enable_vsync", true );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setVSync( enable );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "enable_quad_buffer_support" ) )
  {
    bool enable = resMgr->booleanValue( "OCCViewer", "enable_quad_buffer_support", false );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setQuadBufferSupport( enable );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param.contains( "viewcube" ) )
  {
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setViewCubeParamsFromPreferences();
    }
  }
#endif

  if ( sec == QString( "3DViewer" ) && param == QString( "zooming_mode" ) )
  {
    int mode = resMgr->integerValue( "3DViewer", "zooming_mode", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_OCCVIEWER
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> itOCC( lst );
    while ( itOCC.hasNext() )
    {
      SUIT_ViewModel* vm = itOCC.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setZoomingStyle( mode );
    }
#endif
#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> itVTK( lst );
    while ( itVTK.hasNext() )
    {
      SUIT_ViewModel* vm = itVTK.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setZoomingStyle( mode );
    }
#endif
#endif
  }

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && (param == QString( "speed_value" ) || param == QString( "speed_mode" )) )
  {
    int speed = resMgr->integerValue( "VTKViewer", "speed_value", 10 );
    int mode = resMgr->integerValue( "VTKViewer", "speed_mode", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setIncrementalSpeed( speed, mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "projection_mode" ) )
  {
    int mode = resMgr->integerValue( "VTKViewer", "projection_mode", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setProjectionMode( mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "stereo_type" ) )
  {
    int mode = resMgr->integerValue( "VTKViewer", "stereo_type", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setStereoType( mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "anaglyph_filter" ) )
  {
    int mode = resMgr->integerValue( "VTKViewer", "anaglyph_filter", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setAnaglyphFilter( mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "enable_quad_buffer_support" ) )
  {
    int enable = resMgr->booleanValue( "VTKViewer", "enable_quad_buffer_support", false );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setQuadBufferSupport( enable );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "preselection" ) )
  {
    int mode = resMgr->integerValue( "VTKViewer", "preselection", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setPreSelectionMode( mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "enable_selection" ) )
  {
    bool isToEnableSelection = resMgr->booleanValue( "VTKViewer", "enable_selection", true );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->enableSelection( isToEnableSelection );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && (param == QString( "spacemouse_func1_btn" ) ||
                                         param == QString( "spacemouse_func2_btn" ) ||
                                         param == QString( "spacemouse_func5_btn" ) ) )
  {
    int btn1 = resMgr->integerValue( "VTKViewer", "spacemouse_func1_btn", 1 );
    int btn2 = resMgr->integerValue( "VTKViewer", "spacemouse_func2_btn", 2 );
    int btn3 = resMgr->integerValue( "VTKViewer", "spacemouse_func5_btn", 9 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setSpacemouseButtons( btn1, btn2, btn3 );
    }
#endif
  }
#endif
  if( sec=="ObjectBrowser" )
  {
    SUIT_DataBrowser* ob = objectBrowser();
    if ( !ob )
      return;

    if ( param=="auto_size_first" )
    {
      bool autoSizeFirst = resMgr->booleanValue( "ObjectBrowser", "auto_size_first", true );
      ob->setAutoSizeFirstColumn( autoSizeFirst );
      if ( autoSizeFirst )
        ob->adjustFirstColumnWidth();
    }
    else if ( param=="auto_size" ) {
      bool autoSize = resMgr->booleanValue( "ObjectBrowser", "auto_size", false );
      ob->setAutoSizeColumns(autoSize);
      if ( autoSize )
        ob->adjustColumnsWidth();
    }
    else if ( param=="resize_on_expand_item" ) {
      bool resizeOnExpandItem = resMgr->booleanValue( "ObjectBrowser", "resize_on_expand_item", false );
      ob->setResizeOnExpandItem(resizeOnExpandItem);
    }
    else if ( param == "auto_hide_search_tool" ) {
      ob->searchTool()->enableAutoHide( resMgr->booleanValue( "ObjectBrowser", "auto_hide_search_tool" ) );
    }
  }

  if( sec=="Study" )
  {
    if( param=="auto_save_interval" ) {
      myAutoSaveTimer->stop();
      int autoSaveInterval = resMgr->integerValue( "Study", "auto_save_interval", 0 );
      if ( activeStudy() && autoSaveInterval > 0 ) myAutoSaveTimer->start( autoSaveInterval*60000 );
    }
  }

#ifndef DISABLE_PYCONSOLE
  if( sec=="PyConsole" && pythonConsole() )
  {
    if ( param=="font" ) {
      pythonConsole()->setFont( resMgr->fontValue( "PyConsole", "font" ) );
    }
    else if ( param=="show_banner" ) {
      pythonConsole()->setIsShowBanner( resMgr->booleanValue( "PyConsole", "show_banner", true ) );
    }
    else if ( param=="auto_completion" ) {
      pythonConsole()->setAutoCompletion( resMgr->booleanValue( "PyConsole", "auto_completion", true ) );
    }
  }
#endif

  if( sec=="MRU" )
  {
    QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );
    if ( mru ) {
      if ( param == "visible_count" )
        mru->setVisibleCount( resMgr->integerValue( "MRU", "visible_count", 5 ) );    // 5 MRU items by default
      else if ( param == "max_count" )
        mru->setHistoryCount( resMgr->integerValue( "MRU", "max_count", -1 ) );       // unlimited history by default
      else if ( param == "insert_mode" )
        mru->setInsertMode( resMgr->integerValue( "MRU", "insert_mode", 0 ) );        // QtxMRUAction::MoveFirst by default
      else if ( param == "link_type" )
        mru->setLinkType( resMgr->integerValue( "MRU", "link_type", 0 ) );            // QtxMRUAction::LinkAuto by default
      else if ( param == "show_clear" )
        mru->setClearPossible( resMgr->booleanValue( "MRU", "show_clear", false ) );  // do not show "Clear" item by default
      else if ( param == "show_mru" )
        mru->setVisible( resMgr->booleanValue( "MRU", "show_mru", false ) );          // do not show MRU menu item by default
    }
  }
  if ( sec == "language" && param == "language" )
  {
    // VSR 18.06.2015 : commented out : single warning dialog box is now shown by the LightApp_PreferencesDlg
    //SUIT_MessageBox::information( desktop(), tr( "WRN_WARNING" ), tr( "LANG_CHANGED" ) );
  }
  if ( sec == "language" && param == "locale")
  {
    // VSR 18.06.2015: commented out : single warning dialog box is now shown by the LightApp_PreferencesDlg
    //SUIT_MessageBox::information( desktop(), tr( "WRN_WARNING" ), tr( "LOCALE_CHANGED" ) );
  }
  if ( sec == "desktop" && param == "opaque_resize" ) {
    bool opaqueResize = resMgr->booleanValue( "desktop", "opaque_resize", false );
    // RNV: This code has been commented, because clearing of the QMainWindow::AnimatedDocks option
    //      leads to strange behaviour of the dockable windows (at least at qt-5.6.1):
    //      any dockable window can't be docked to the another area, except initial area.
    //      It is possible to move window to another area, but it always returns to the initial area.
    //
    //    QMainWindow::DockOptions dopts = desktop()->dockOptions();
    //    if ( opaqueResize ) dopts |= QMainWindow::AnimatedDocks;
    //     else                dopts &= ~QMainWindow::AnimatedDocks;
    //    desktop()->setDockOptions( dopts );
    desktop()->setOpaqueResize( opaqueResize );
    if ( dynamic_cast<STD_TabDesktop*>( desktop() ) )
      dynamic_cast<STD_TabDesktop*>( desktop() )->workstack()->setOpaqueResize( opaqueResize );
  }

#ifndef DISABLE_PLOT2DVIEWER
  QList<SUIT_ViewManager*> lst;
  viewManagers( Plot2d_Viewer::Type(), lst );
  QListIterator<SUIT_ViewManager*> itPlot2d( lst );
  while ( itPlot2d.hasNext() ) {
    SUIT_ViewManager* viewMgr = itPlot2d.next();
    SUIT_ViewModel* vm = viewMgr->getViewModel();
    if ( !vm || !vm->inherits( "Plot2d_Viewer" ) )
      continue;

    //Plot2d_Viewer* Plot2dVM = dynamic_cast<Plot2d_Viewer*>( vm );

    viewMgr->setViewModel( vm  );
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( viewMgr->getActiveView() );
    if( wnd ) {
      Plot2d_ViewFrame* frame = wnd->getViewFrame();
      frame->SetPreference();
    }
  }
#endif
}

/*!
  Loads preferences
*/
void LightApp_Application::loadPreferences()
{
  CAM_Application::loadPreferences();

  SUIT_ResourceMgr* aResMgr = resourceMgr();

  if ( !aResMgr )
    return;

  static bool mru_load = true;
  if ( mru_load )
  {
    QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );
    if ( mru ) {
      mru->setVisible( aResMgr->booleanValue( "MRU", "show_mru", false ) );         // do not show MRU menu item by default
      mru->setVisibleCount( aResMgr->integerValue( "MRU", "visible_count", 5 ) );   // 5 MRU items by default
      mru->setHistoryCount( aResMgr->integerValue( "MRU", "max_count", -1 ) );      // unlimited history by default
      mru->setInsertMode( aResMgr->integerValue( "MRU", "insert_mode", 0 ) );       // QtxMRUAction::MoveFirst by default
      mru->setLinkType( aResMgr->integerValue( "MRU", "link_type", 0 ) );           // QtxMRUAction::LinkAuto by default
      mru->setClearPossible( aResMgr->booleanValue( "MRU", "show_clear", false ) ); // do not show "Clear" item by default
      mru->loadLinks( aResMgr, "MRU" );
    }
    mru_load = false;
  }

  myWinVis.clear();
  QStringList mods = aResMgr->parameters( "windows_visibility" );
  for ( QStringList::const_iterator itr = mods.begin(); itr != mods.end(); ++itr )
  {
    QByteArray arr;
    if ( aResMgr->value( "windows_visibility", *itr, arr ) )
      myWinVis.insert( *itr, arr );
  }

  if ( desktop() ) {
    desktop()->retrieveGeometry( aResMgr->stringValue( "desktop", "geometry" ) );
    bool opaqueResize = aResMgr->booleanValue( "desktop", "opaque_resize", false );
    //    QMainWindow::DockOptions dopts = desktop()->dockOptions();
    //    if ( opaqueResize ) dopts |= QMainWindow::AnimatedDocks;
    //    else                dopts &= ~QMainWindow::AnimatedDocks;
    //    desktop()->setDockOptions( dopts );
    desktop()->setOpaqueResize( opaqueResize );
    if ( dynamic_cast<STD_TabDesktop*>( desktop() ) )
      dynamic_cast<STD_TabDesktop*>( desktop() )->workstack()->setOpaqueResize( opaqueResize );
  }
}

/*!
  Saves preferences
*/
void LightApp_Application::savePreferences()
{
  CAM_Application::savePreferences();

  saveDockWindowsState();

  SUIT_ResourceMgr* aResMgr = resourceMgr();

  if ( !aResMgr )
    return;

  QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );
  if ( mru )
    mru->saveLinks( aResMgr, "MRU" );

  for ( WinVis::const_iterator itr = myWinVis.begin(); itr != myWinVis.end(); ++itr )
    aResMgr->setValue( "windows_visibility", itr.key(), itr.value() );

  if ( desktop() )
    aResMgr->setValue( "desktop", "geometry", desktop()->storeGeometry() );

#if GUI_DEVELOPMENT > 0
  aResMgr->setValue( "salome", "version", QString(GUI_VERSION_STR)+"dev" );
#else
  aResMgr->setValue( "salome", "version", QString(GUI_VERSION_STR) );
#endif

  aResMgr->save();
}

/*!
  Updates desktop title
*/
void LightApp_Application::updateDesktopTitle()
{
  QString aTitle = applicationName();
  QString aVer = applicationVersion();
  if ( !aVer.isEmpty() )
    aTitle += QString( " " ) + aVer;

  if ( activeStudy() ) {
    QString sName = SUIT_Tools::file( activeStudy()->studyName().trimmed(), false );
    aTitle += QString( " - [%1]" ).arg( sName );
  }

  desktop()->setWindowTitle( aTitle );
}

/*!
  \brief Get map of the operations which can be performed
  on the module activation.

  The method should return the map of the kind \c {<id>:<name>}
  where \c <id> is an integer identifier of the operation and
  \c <name> is a title for the button to be added to the
  dialog box. After user selects the required operation by the
  clicking the corresponding button in the dialog box, its identifier
  is passed to the moduleActionSelected() method to process
  the made choice.

  \return map of the operations
  \sa moduleActionSelected()
*/
QMap<int, QString> LightApp_Application::activateModuleActions() const
{
  QMap<int, QString> opmap;
  opmap.insert( NewStudyId,  tr( "ACTIVATE_MODULE_OP_NEW" ) );
  opmap.insert( OpenStudyId, tr( "ACTIVATE_MODULE_OP_OPEN" ) );
  return opmap;
}

/*!
  \brief Called when the used selectes required operation chosen
  from "Activate module" dialog box.

  Performs the required operation according to the user choice.

  \param id operation identifier
  \sa activateModuleActions()
*/
void LightApp_Application::moduleActionSelected( const int id )
{
  switch ( id ) {
  case NewStudyId:
    onNewDoc();
    break;
  case OpenStudyId:
    onOpenDoc();
    break;
  default:
    break;
  }
}

/*!
  Updates windows after close document
*/
void LightApp_Application::afterCloseDoc()
{
  updateWindows();

  CAM_Application::afterCloseDoc();
}

/*!
  Updates actions of active module
*/
void LightApp_Application::updateModuleActions()
{
  emit moduleActivated( activeModule() ? activeModule()->moduleName() : QString() );
}

bool LightApp_Application::checkModule( const QString& title )
{
  if ( title.isEmpty() )
    return false;

  QString library = moduleLibrary( title, true );
  if ( library.isEmpty() )
    return false;

  QString name = moduleName( title );

  bool isPyModule = library.contains( "SalomePyQtGUI" ) || library.contains( "SalomePyQtGUILight" );

  QStringList paths;
#if defined(WIN32)
  paths = QString( Qtx::getenv( "PATH" ) ).split( ";", QString::SkipEmptyParts );
#elif defined(__APPLE__)
  paths = QString( Qtx::getenv( "DYLD_LIBRARY_PATH" ) ).split( ":", QString::SkipEmptyParts );
#else
  paths = QString( Qtx::getenv( "LD_LIBRARY_PATH" ) ).split( ":", QString::SkipEmptyParts );
#endif

  bool isFound = false;
  QStringList::const_iterator it;
  for ( it = paths.begin(); it != paths.end() && !isFound; ++it )
  {
    isFound = QFileInfo( Qtx::addSlash( *it ) + library ).exists();
  }

  if ( !isFound )
  {
    INFOS( std::endl <<
           "****************************************************************" << std::endl <<
           "     Warning: library " << qPrintable( library ) << " is not found!" << std::endl <<
           "     Module " << qPrintable( title ) << " will not be available in GUI mode!" << std::endl <<
           "****************************************************************" << std::endl);
    return false;
  }

  if ( isPyModule )
  {
    QString pyModule = QString( "%1GUI.py" ).arg( name );
#if defined(WIN32)
    paths = QString( Qtx::getenv( "PYTHONPATH" ) ).split( ";", QString::SkipEmptyParts );
#else
    paths = QString( Qtx::getenv( "PYTHONPATH" ) ).split( ":", QString::SkipEmptyParts );
#endif
    isFound = false;
    for ( it = paths.begin(); it != paths.end() && !isFound; ++it )
    {
      isFound = QFileInfo( Qtx::addSlash( *it ) + pyModule ).exists();
    }

    if ( !isFound )
    {
      INFOS( std::endl <<
             "****************************************************************" << std::endl <<
             "     Warning: Python module " << qPrintable( pyModule ) << " is not found!" << std::endl <<
             "     Module " << qPrintable( title ) << " will not be available in GUI mode!" << std::endl <<
             "****************************************************************" << std::endl);
      return false;
    }
  }

  return true;
}

/*!
  Gets current windows.
  \param winMap - output current windows map.
*/
void LightApp_Application::currentWindows( QMap<int, int>& winMap ) const
{
  winMap.clear();
  if ( activeModule() && activeModule()->inherits( "LightApp_Module" ) )
    ((LightApp_Module*)activeModule())->windows( winMap );
  else
    defaultWindows( winMap );
}

/*!
  Gets current view managers.
  \param lst - output current view managers list.
*/
void LightApp_Application::currentViewManagers( QStringList& lst ) const
{
  lst.clear();
  if ( !activeStudy() )
    return;

  if ( activeModule() && activeModule()->inherits( "LightApp_Module" ) )
    ((LightApp_Module*)activeModule())->viewManagers( lst );
  else
    defaultViewManagers( lst );
}

/*!
  Updates windows
*/
void LightApp_Application::updateWindows()
{
  QMap<int, int> winMap;
  currentWindows( winMap );

  for ( QMap<int, int>::ConstIterator it = winMap.begin(); it != winMap.end(); ++it )
  {
    if ( !dockWindow( it.key() ) )
      getWindow( it.key() );
  }

  for ( WinMap::ConstIterator it = myWin.begin(); it != myWin.end(); ++it )
  {
    QWidget* wid = it.value();
    if ( !wid )
      continue;
    if ( winMap.contains( it.key() ) )
      wid->setVisible( true );
    else if ( !activeStudy() )
      delete wid;
    else
      wid->setVisible( false );
  }

  loadDockWindowsState();

  if ( !activeModule() && infoPanel() )
  {
    infoPanel()->clear();
    infoPanel()->setTitle( tr( "INFO_WELCOME_TO_SALOME" ) );

    int grp = infoPanel()->addGroup( tr( "INFO_GETTING_STARTED" ) );
    infoPanel()->addAction( action( FileNewId ), grp );
    infoPanel()->addLabel( action( FileNewId )->statusTip(), grp );
    infoPanel()->addAction( action( FileOpenId ), grp );
    infoPanel()->addLabel( action( FileOpenId )->statusTip(), grp );
    if ( HAS_TUTORIAL_URL ) {
      infoPanel()->addAction( action( TutorialsId ), grp );
      infoPanel()->addLabel( action( TutorialsId )->statusTip(), grp );
    }
    infoPanel()->addAction( action( VideosId ), grp );
    infoPanel()->addLabel( action( VideosId )->statusTip(), grp );

    LightApp_ModuleAction* ma = qobject_cast<LightApp_ModuleAction*>(action(ModulesListId));
    if ( ma && ma->count() > 0 )
    {
      grp = infoPanel()->addGroup( tr( "INFO_AVAILABLE_MODULES" ) );
      foreach ( QString mname, ma->modules() )
      {
        infoPanel()->addAction( ma->moduleAction( mname ), grp );
        if ( !moduleDescription( mname ).isEmpty() )
          infoPanel()->addLabel( moduleDescription( mname ), grp );
      }
    }
  }
}

/*!
  Updates view managers
*/
void LightApp_Application::updateViewManagers()
{
  QStringList lst;
  currentViewManagers( lst );

  for ( QStringList::const_iterator it = lst.begin(); it != lst.end(); ++it )
    getViewManager( *it, true );
}

/*!
  Loads windows geometry
*/
void LightApp_Application::loadDockWindowsState()
{
  if ( !desktop() )
    return;
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  bool storeWin = aResMgr->booleanValue( "Study", "store_positions", true );
  bool storeTb = aResMgr->booleanValue( "Study", "store_tool_positions", true );

  QString modName;
  if ( activeModule() )
    modName = activeModule()->name();
  else if ( activeStudy() )
    modName = "nomodule";

  QtxResourceMgr::WorkingMode prevMode = aResMgr->setWorkingMode( QtxResourceMgr::IgnoreUserValues );
  QByteArray aDefaultState;
  aResMgr->value( "windows_geometry", modName, aDefaultState );
  QByteArray aDefaultVisibility;
  aResMgr->value( "windows_visibility", modName, aDefaultVisibility );
  bool hasDefaultVisibility = !aDefaultVisibility.isEmpty();
  aResMgr->setWorkingMode( prevMode );

  if( !storeWin && !storeTb && aDefaultState.isEmpty() && !hasDefaultVisibility)
    return;

  if ( aResMgr->hasValue("windows_geometry" ,modName ) ) {
    long version = Qtx::versionToId( aResMgr->stringValue( "windows_geometry_version", modName, "" ) );
    QByteArray arr;
    if ( version > Qtx::versionToId( "7.4.1" ) )
      aResMgr->value( "windows_geometry", modName , arr );
    else
      arr = aDefaultState;
    QByteArray aTargetArray = processState(arr, storeWin, storeTb, true, aDefaultState);
    desktop()->restoreState( aTargetArray );
  }

  QStringList mainToolbarsNames;
  mainToolbarsNames << "SalomeStandard" << "SalomeModules";
  QList<QToolBar*> mainToolbars = findToolBars( mainToolbarsNames );
  foreach( QToolBar* tb, mainToolbars ) tb->setVisible( true );
  /*
  if ( !myWinVis.contains( modName ) && aDefaultVisibility.isEmpty())
    return;

  QMap<QString, bool> *tbMap = 0;
  QMap<QString, bool> *dwMap = 0;

  QMap<QString, bool> userTbMap, userDwMap;
  dockWindowsState( myWinVis[modName], userTbMap, userDwMap );

  QMap<QString, bool> defaultTbMap, defaultDwMap;
  if(hasDefaultVisibility) {
    dockWindowsState( aDefaultVisibility, defaultTbMap, defaultDwMap);
  }

  if(storeTb) {
    tbMap =  &userTbMap;
  } else {
    if(hasDefaultVisibility){
      tbMap =  &defaultTbMap;
    }
  }

  if(storeWin) {
    dwMap =  &userDwMap;
  } else {
    if(hasDefaultVisibility){
      dwMap =  &defaultDwMap;
    }
  }

  if(tbMap) {
    QList<QToolBar*> tbList = findToolBars();
    for ( QList<QToolBar*>::iterator tit = tbList.begin(); tit != tbList.end(); ++tit )
      {
        QToolBar* tb = *tit;
        if ( tbMap->contains( tb->objectName() ) ) {
          tb->setVisible( (*tbMap)[tb->objectName()] );
        }
      }
  }

  if(dwMap) {
    QList<QDockWidget*> dwList = desktop()->findChildren<QDockWidget*>();
    for ( QList<QDockWidget*>::iterator dit = dwList.begin(); dit != dwList.end(); ++dit )
      {
        QDockWidget* dw = *dit;

        QObject* po = Qtx::findParent( dw, "QMainWindow" );
        if ( po != desktop() )
          continue;

        if ( dwMap->contains( dw->objectName() ) )
          dw->setVisible( (*dwMap)[dw->objectName()] );
      }
  }
  */
}


/*!
  Saves windows geometry
*/
void LightApp_Application::saveDockWindowsState()
{
  if ( !desktop() )
    return;

  bool storeWin = resourceMgr()->booleanValue( "Study", "store_positions", true );
  bool storeTb = resourceMgr()->booleanValue( "Study", "store_tool_positions", true );

  if( !storeWin && !storeTb )
    return;

  QString modName;
  if ( activeModule() )
    modName = activeModule()->name();
  else if ( activeStudy() )
    modName = "nomodule";

  QString versionId = GUI_VERSION_STR;
#if GUI_DEVELOPMENT > 0
  versionId += "dev";
#endif

  QByteArray arr = desktop()->saveState();
  resourceMgr()->setValue( "windows_geometry", modName, processState(arr, storeWin, storeTb, false) );
  resourceMgr()->setValue( "windows_geometry_version", modName, versionId );

  QByteArray visArr;
  if ( myWinVis.contains( modName ) )
    visArr = myWinVis[modName];

  QMap<QString, bool> tbMap, dwMap;
  dockWindowsState( visArr, tbMap, dwMap );

  QList<QToolBar*> tbList = desktop()->findChildren<QToolBar*>();
  for ( QList<QToolBar*>::iterator it = tbList.begin(); it != tbList.end(); ++it )
  {
    QToolBar* tb = *it;
    tbMap.insert( tb->objectName(), tb->toggleViewAction()->isChecked() );
  }

  QList<QDockWidget*> dwList = desktop()->findChildren<QDockWidget*>();
  for ( QList<QDockWidget*>::iterator it = dwList.begin(); it != dwList.end(); ++it )
  {
    QDockWidget* wid = *it;
    dwMap.insert( wid->objectName(), wid->toggleViewAction()->isChecked() );
  }

  visArr = dockWindowsState( tbMap, dwMap );

  myWinVis.insert( modName, visArr );
}

QByteArray LightApp_Application::dockWindowsState( const QMap<QString, bool>& tb, const QMap<QString, bool>& dw ) const
{
  QByteArray visArr;
  QDataStream stream( &visArr, QIODevice::WriteOnly );

  stream << (uchar)ToolBarMarker;
  stream << tb.size();
  for ( QMap<QString, bool>::const_iterator tit = tb.begin(); tit != tb.end(); ++tit )
  {
    stream << tit.key();
    stream << (uchar)( tit.value() ? 1 : 0 );
  }

  stream << (uchar)DockWidgetMarker;
  stream << dw.size();
  for ( QMap<QString, bool>::const_iterator wit = dw.begin(); wit != dw.end(); ++wit )
  {
    stream << wit.key();
    stream << (uchar)( wit.value() ? 1 : 0 );
  }

  return visArr;
}

void LightApp_Application::dockWindowsState( const QByteArray& arr, QMap<QString, bool>& tb, QMap<QString, bool>& dw ) const
{
  tb.clear();
  dw.clear();

  QByteArray visArr = arr;
  QDataStream stream( &visArr, QIODevice::ReadOnly );

  uchar marker;
  stream >> marker;
  if ( marker != ToolBarMarker )
    return;

  int lines;
  stream >> lines;
  for ( int i = 0; i < lines; ++i )
  {
    QString objectName;
    stream >> objectName;
    uchar shown;
    stream >> shown;
    tb.insert( objectName, shown );
  }

  stream >> marker;
  if ( marker != DockWidgetMarker )
    return;

  stream >> lines;
  for ( int j = 0; j < lines; ++j )
  {
    QString objectName;
    stream >> objectName;
    uchar shown;
    stream >> shown;
    dw.insert( objectName, shown );
  }
}

QPixmap LightApp_Application::moduleIcon( const QString& moduleTitle, const int size ) const
{
  QPixmap icon;
  if ( resourceMgr() )
  {
    QPixmap defaultIcon = resourceMgr()->loadPixmap( "LightApp", tr( "APP_MODULE_ICO" ), QPixmap( imageEmptyIcon ) );
    QString iconName = resourceMgr()->stringValue( moduleName( moduleTitle ), "icon", QString() );
    icon = resourceMgr()->loadPixmap( moduleName( moduleTitle ), iconName, defaultIcon );
    if ( size > 0 )
      icon = Qtx::scaleIcon( icon, size );
  }
  return icon;
}

/*!
  Inserts items in popup, which necessary for current application
*/
void LightApp_Application::contextMenuPopup( const QString& type, QMenu* thePopup, QString& title )
{
  //Add "Rename" item
  LightApp_SelectionMgr* selMgr = LightApp_Application::selectionMgr();
  bool cacheIsOn = selMgr->isSelectionCacheEnabled();
  selMgr->setSelectionCacheEnabled( true );

  SUIT_DataBrowser* ob = objectBrowser();

  CAM_Application::contextMenuPopup( type, thePopup, title );

  if ( ob && type == ob->popupClientType() ) {
    thePopup->addSeparator();
    QAction* a = thePopup->addAction( tr( "MEN_REFRESH" ), this, SLOT( onRefresh() ) );
    if ( ob->shortcutKey(SUIT_DataBrowser::UpdateShortcut) )
      a->setShortcut( ob->shortcutKey(SUIT_DataBrowser::UpdateShortcut) );
  }

#ifndef DISABLE_SALOMEOBJECT
  if ( selMgr && ob ) {
    SALOME_ListIO selected;
    selMgr->selectedObjects( selected );
    if(selected.Extent() == 1){
      Handle(SALOME_InteractiveObject) anIObject = selected.First();
      SUIT_DataObject* obj = findObject(anIObject->getEntry());
      if(obj && obj->renameAllowed()) {
        QAction* a = new QAction(tr("MEN_RENAME_OBJ"), thePopup);
        connect( a, SIGNAL( triggered(bool) ), ob, SLOT( onStartEditing() ) );
        if ( ob->shortcutKey(SUIT_DataBrowser::RenameShortcut) )
          a->setShortcut( ob->shortcutKey(SUIT_DataBrowser::RenameShortcut) );

        QList<QAction*> acts = thePopup->actions();
        QAction* firstAction = acts.count() > 0 ? acts.first() : 0;
        thePopup->insertAction(firstAction,a);
      }
    }
  }
#endif

  selMgr->setSelectionCacheEnabled( cacheIsOn );
}

/*!
  Create empty study
*/
void LightApp_Application::createEmptyStudy()
{
  CAM_Application::createEmptyStudy();

  if ( objectBrowser() )
    objectBrowser()->updateTree();

  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  if ( aResMgr && activeStudy() ) {
    int autoSaveInterval = aResMgr->integerValue( "Study", "auto_save_interval", 0 );
    if ( autoSaveInterval > 0 ) myAutoSaveTimer->start( autoSaveInterval*60000 );
  }
}

/*!Set desktop:*/
void LightApp_Application::setDesktop( SUIT_Desktop* desk )
{
  CAM_Application::setDesktop( desk );

  if ( desk ) {
    connect( desk, SIGNAL( message( const QString& ) ),
             this, SLOT( onDesktopMessage( const QString& ) ), Qt::UniqueConnection );
    connect( desk, SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
             this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ), Qt::UniqueConnection );
    /* connect( desk, SIGNAL( windowMoved( SUIT_ViewWindow* ) ),
             this, SLOT( onWindowMoved( SUIT_ViewWindow* ) ), Qt::UniqueConnection ); */
  }
}

/*!
  Activates module
  \param mod - module to be activated
*/
bool LightApp_Application::activateModule( CAM_Module* mod )
{
  bool res = CAM_Application::activateModule( mod );

  if ( objectBrowser() )
    objectBrowser()->updateTree();

  return res;
}

/*!
  \return keyborad accelerators manager object
*/
SUIT_Accel* LightApp_Application::accel() const
{
  return myAccel;
}

/*!
  Removes dead widget container from map
*/
void LightApp_Application::onWCDestroyed( QObject* ob )
{
  // remove destroyed widget container from windows map
  for ( WinMap::ConstIterator itr = myWin.begin(); itr != myWin.end(); ++itr )
  {
    if ( itr.value() != ob )
      continue;

    int key = itr.key();
    myWin.remove( key );
    break;
  }
}

void LightApp_Application::onMRUActivated( const QString& name )
{
  SUIT_Session* s = SUIT_Session::session();
  if ( s && s->activeApplication() == this )
    onOpenDoc( name );
}

void LightApp_Application::onStylePreferences()
{
#ifdef USE_SALOME_STYLE
  Style_PrefDlg dlg( desktop() );
  dlg.exec();

  resourceMgr()->setValue( "Style", "use_salome_style", Style_Salome::isActive() );
#endif // USE_SALOME_STYLE
}

void LightApp_Application::onFullScreen(){
  if(myScreenHelper) {
    if(desktop()->isFullScreen())
      myScreenHelper->switchToNormalScreen();
    else
      myScreenHelper->switchToFullScreen();
  }
}

/*!
  Connects just added view manager
*/
void LightApp_Application::addViewManager( SUIT_ViewManager* vm )
{
  connect( vm, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
           this, SLOT( onCloseView( SUIT_ViewManager* ) ) );
  CAM_Application::addViewManager( vm );
}

/*!
  Remove view manager from memory
*/
void LightApp_Application::removeViewManager( SUIT_ViewManager* vm )
{
  disconnect( vm, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
           this, SLOT( onCloseView( SUIT_ViewManager* ) ) );
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(activeStudy());
  if (aStudy )
    aStudy->removeObjectProperties( vm->getGlobalId() );

  CAM_Application::removeViewManager( vm );

  LightApp_SelectionMgr* selMgr = selectionMgr();
  QList<SUIT_Selector*> selectors;
  selMgr->selectors( selectors );
  foreach( SUIT_Selector* selector, selectors ) {
    if ( selector->owner() == vm->getViewModel() ) {
      delete selector;
    }
  }

  // IPAL22894: Crash on closing OCC view
  //delete vm;
  vm->deleteLater();
}

/*!
  Renames active window of desktop
*/
void LightApp_Application::onRenameWindow()
{
  if( !desktop() )
    return;

  QWidget* w = desktop()->activeWindow();
  if( !w )
    return;

  bool ok;
  QString name = QInputDialog::getText( w, tr( "TOT_RENAME" ), tr( "PRP_RENAME" ), QLineEdit::Normal, w->windowTitle(), &ok );
  if( ok && !name.isEmpty() )
    w->setWindowTitle( name );
}

/*!
  Closes active window of desktop
*/
void LightApp_Application::onCloseWindow()
{
  if( !desktop() )
    return;

  QWidget* w = desktop()->activeWindow();
  if( !w )
    return;

  w->close();
}

/*!
  Closes all windows of desktop
*/
void LightApp_Application::onCloseAllWindow()
{
  STD_TabDesktop* desk = dynamic_cast<STD_TabDesktop*>( desktop() );
  if( !desk )
    return;

  QList<SUIT_ViewWindow*> wndList = desk->windows();
  SUIT_ViewWindow* wnd;
  foreach( wnd, wndList )
  {
    if ( wnd )
      wnd->close();
  }
}

/*!
  Groups all windows of desktop
*/
void LightApp_Application::onGroupAllWindow()
{
  STD_TabDesktop* desk = dynamic_cast<STD_TabDesktop*>( desktop() );
  if( !desk )
    return;

  QtxWorkstack* wgStack = desk->workstack();
  if ( wgStack )
    wgStack->stack();
}

/*!
  \return default name for an active study
*/
void LightApp_Application::setDefaultStudyName( const QString& theName )
{
  QStringList anInfoList;
  modules( anInfoList, false );

  LightApp_Study* aStudy = (LightApp_Study*)activeStudy();
  if( anInfoList.count() == 1 && // to avoid a conflict between different modules
      !aStudy->isSaved() )
  {
    aStudy->setStudyName( theName );
    updateDesktopTitle();
  }
}

/*!
  Custom event handler
*/
bool LightApp_Application::event( QEvent* e )
{
  if( e && e->type()==2000 )
  {
    SALOME_CustomEvent* ce = ( SALOME_CustomEvent* )e;
    QString* d = ( QString* )ce->data();
    if( SUIT_MessageBox::question(0, tr("WRN_WARNING"),
                                  d ? *d : "",
                                  SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                  SUIT_MessageBox::Yes ) == SUIT_MessageBox::Yes )
    {
      QStringList path;
      path << tr( "PREF_CATEGORY_SALOME" ) << tr( "PREF_TAB_GENERAL" )
           << tr( "PREF_GROUP_EXT_BROWSER" ) << tr( "PREF_APP" );
      showPreferences( path );
    }
    if( d )
      delete d;
    return true;
  }
  return CAM_Application::event( e );
}

/*! Check data object */
bool LightApp_Application::checkDataObject(LightApp_DataObject* theObj)
{
  if (theObj)
    {
      bool isSuitable = !theObj->entry().isEmpty() &&
                        !theObj->componentDataType().isEmpty() &&
                        !theObj->name().isEmpty();
      return isSuitable;
    }

  return false;
}

int LightApp_Application::openChoice( const QString& aName )
{
  int choice = CAM_Application::openChoice( aName );

  if ( choice == OpenExist ) // The document is already open.
  {
    // Do you want to reload it?
    if ( SUIT_MessageBox::question( desktop(), tr( "WRN_WARNING" ), tr( "QUE_DOC_ALREADYOPEN" ).arg( aName ),
                                    SUIT_MessageBox::Yes | SUIT_MessageBox::No, SUIT_MessageBox::No ) == SUIT_MessageBox::Yes )
      choice = OpenReload;
  }

  return choice;
}

bool LightApp_Application::openAction( const int choice, const QString& aName )
{
  bool res = false;
  switch ( choice )
  {
  case OpenReload:
    {
      STD_Application* app = 0;
      SUIT_Session* session = SUIT_Session::session();
      QList<SUIT_Application*> appList = session->applications();
      for ( QList<SUIT_Application*>::iterator it = appList.begin(); it != appList.end() && !app; ++it )
      {
        if ( (*it)->activeStudy() && (*it)->activeStudy()->studyName() == aName )
          app = ::qobject_cast<STD_Application*>( *it );
      }

      if ( app )
      {
        app->onCloseDoc( false );
        appList = session->applications();
        STD_Application* other = 0;
        for ( QList<SUIT_Application*>::iterator it = appList.begin(); it != appList.end() && !other; ++it )
          other = ::qobject_cast<STD_Application*>( *it );

        if ( other )
          res = other->onOpenDoc( aName );
      }
    }
    break;
  default:
    res = CAM_Application::openAction( choice, aName );
    break;
  }

  return res;
}

QStringList LightApp_Application::viewManagersTypes() const
{
  QStringList aTypesList;
  aTypesList += myUserWmTypes;
#ifndef DISABLE_GLVIEWER
  aTypesList<<GLViewer_Viewer::Type();
#endif
#ifndef DISABLE_PLOT2DVIEWER
  aTypesList<<Plot2d_Viewer::Type();
#endif
#ifndef DISABLE_QXGRAPHVIEWER
  aTypesList<<QxScene_Viewer::Type();
#endif
#ifndef DISABLE_PVVIEWER
  aTypesList<<PVViewer_Viewer::Type();
#endif
#ifndef DISABLE_PYVIEWER
  aTypesList<<PyViewer_Viewer::Type();
#endif
#ifndef DISABLE_OCCVIEWER
  aTypesList<<OCCViewer_Viewer::Type();
#endif
#ifndef DISABLE_VTKVIEWER
 #ifndef DISABLE_SALOMEOBJECT
  aTypesList<<SVTK_Viewer::Type();
 #else
  aTypesList<<VTKViewer_Viewer::Type();
 #endif
#endif
#ifndef DISABLE_PV3DVIEWER
  aTypesList<<PV3DViewer_ViewModel::Type();
#endif
  return aTypesList;
}
/*!
 * Removes all view managers of known types
 * Other view managers are ignored
 */
void LightApp_Application::clearKnownViewManagers()
{
  QStringList aTypesList = viewManagersTypes();
  QList<SUIT_ViewManager*> aMgrList;
  viewManagers( aMgrList );
  foreach (SUIT_ViewManager* aMgr, aMgrList) {
    if (aTypesList.contains(aMgr->getType()))
      removeViewManager(aMgr);
  }
}

/*!
  Copy of current selection
 */
void LightApp_Application::onCopy()
{
  LightApp_Module* m = dynamic_cast<LightApp_Module*>( activeModule() );
  if( m )
    m->copy();
}

/*!
  Paste of current data in clipboard
 */
void LightApp_Application::onPaste()
{
  LightApp_Module* m = dynamic_cast<LightApp_Module*>( activeModule() );
  if( m )
    m->paste();
}

/*!
  Browse (i.e. set focus on) the published objects
  \param theIsApplyAndClose - flag indicating that the dialog for creating objects
                              has been accepted by Ok (or Apply & Close) button
  \param theIsOptimizedBrowsing - flag switching to optimized browsing mode
                                  (to select the first published object only)
  \return entry of the selected object
 */
QString LightApp_Application::browseObjects( const QStringList& theEntryList,
                                             const bool theIsApplyAndClose,
                                             const bool theIsOptimizedBrowsing )
{
  QString aResult;
  if( SUIT_ResourceMgr* aResourceMgr = resourceMgr() )
  {
    int aBrowsePolicy = aResourceMgr->integerValue( "ObjectBrowser", "browse_published_object", (int)BP_Never );
    switch( aBrowsePolicy )
    {
      case BP_Never:
        return aResult;
      case BP_ApplyAndClose:
        if( !theIsApplyAndClose )
          return aResult;
      case BP_Always:
      default:
        break;
    }
  }

  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( activeStudy() );
  if( !aStudy )
    return aResult;

  SUIT_DataBrowser* anOB = objectBrowser();
  if( !anOB )
    return aResult;

  SUIT_AbstractModel* aModel = dynamic_cast<SUIT_AbstractModel*>( anOB->model() );
  if( !aModel )
    return aResult;

  QStringListIterator anIter( theEntryList );
  if( theIsOptimizedBrowsing )
  {
    // go to the last entry
    anIter.toBack();
    if( anIter.hasPrevious() )
      anIter.previous();
  }

  // scroll to each entry in the list
  // (in optimized mode - to the last entry only)
  QString anEntry;
  LightApp_DataObject* anObject = 0;
  while( anIter.hasNext() )
  {
    anEntry = anIter.next();
    if( !anEntry.isEmpty() )
    {
      anObject = aStudy->findObjectByEntry( anEntry );
      if( anObject )
      {
        QModelIndex anIndex = aModel->index( anObject );
        anOB->treeView()->scrollTo( anIndex );
      }
    }
  }

  // always select the last object
  if( anObject && !anEntry.isEmpty() )
  {
    QList<SUIT_Selector*> aSelectorList;
    selectionMgr()->selectors( "ObjectBrowser", aSelectorList );
    if( !aSelectorList.isEmpty() )
    {
      if( LightApp_OBSelector* aSelector = dynamic_cast<LightApp_OBSelector*>( aSelectorList.first() ) )
      {
        bool anIsAutoBlock = aSelector->autoBlock();

        // temporarily disable auto block, to emit LightApp_SelectionMgr::currentSelectionChanged() signal
        aSelector->setAutoBlock( false );

        SUIT_DataOwnerPtrList aList;
#ifndef DISABLE_SALOMEOBJECT
        Handle(SALOME_InteractiveObject) aSObj = new SALOME_InteractiveObject
          ( anObject->entry().toUtf8().constData(),
            anObject->componentDataType().toLatin1().constData(),
            anObject->name().toUtf8().constData() );
        LightApp_DataOwner* owner = new LightApp_DataOwner( aSObj  );
#else
        LightApp_DataOwner* owner = new LightApp_DataOwner( anEntry );
#endif

        aList.append( owner );
        selectionMgr()->setSelected( aList );
        aResult = anEntry;

        // restore auto block flag
        aSelector->setAutoBlock( anIsAutoBlock );
      }
    }
  }

  return aResult;
}

SUIT_DataObject* LightApp_Application::findObject( const QString& id ) const
{
  LightApp_Study* study = dynamic_cast<LightApp_Study*>( activeStudy() );
  return study ? study->findObjectByEntry( id ) : 0;
}

/*!
  Checks that an object can be renamed.
  \param entry entry of the object
  \brief Return \c true if object can be renamed
*/
bool LightApp_Application::renameAllowed( const QString& /*entry*/) const {
  return false;
}

/*!
  Rename object by entry.
  \param entry entry of the object
  \param name new name of the object
  \brief Return \c true if rename operation finished successfully, \c false otherwise.
*/
bool LightApp_Application::renameObject( const QString& /*entry*/, const QString& /*name*/ ) {
  return false;
}

/*! Process standard messages from desktop */
void LightApp_Application::onDesktopMessage( const QString& message )
{
  const QString sectionSeparator = "/";

  if ( message.toLower() == "updateobjectbrowser" ||
       message.toLower() == "updateobjbrowser" ) {
    // update object browser
    updateObjectBrowser();
  }
  else if ( message.toLower().startsWith( "activate_viewer" ) ) {
    QString vtype = message.split( sectionSeparator ).last();
    if ( !vtype.isEmpty() )
      getViewManager( vtype, true );
  }
  else if ( message.toLower().startsWith("register_module_in_study" ) ) {
    QString moduleName = message.split( sectionSeparator ).last();
    // Check name of current activating module name in order to avoid ciclik
    // call because of messages
    if (!property("activateModule").toBool()) {
      CAM_Module* mod = module(moduleName);
      if (!mod)
        mod = module(moduleTitle(moduleName));
      if (!mod) {
        mod = loadModule(moduleName);
        if (!mod)
          mod = loadModule(moduleTitle(moduleName));
        if (mod) {
          addModule(mod);
        }
      }
      if (mod) {
        CAM_Study* anActiveStudy = dynamic_cast<CAM_Study*>(activeStudy());
        if (anActiveStudy) {
          mod->connectToStudy(anActiveStudy);
          LightApp_DataModel* aDM = dynamic_cast<LightApp_DataModel*>(mod->dataModel());
          if(aDM) {
            aDM->initRootObject();
          }
        }
      }
    }
  }
  else {
    QStringList data = message.split( sectionSeparator );
    if ( data.count() > 1 ) {
      QString msgType = data[0].trimmed();
      LightApp_Module* sMod = 0;
      CAM_Module* mod = module( msgType );
      if ( !mod )
        mod = module( moduleTitle( msgType ) );
      if ( mod && mod->inherits( "LightApp_Module" ) )
        sMod = (LightApp_Module*)mod;

      if ( msgType.toLower() == "preferences" ) {
        // requested preferences change: should be given as "preferences/<section>/<name>/<value>"
        // for example "preferences/Study/multi_file_dump/true"
        if ( data.count() > 3 ) {
          QString section = data[1].trimmed();
          QString param   = data[2].trimmed();
          QString value   = QStringList( data.mid(3) ).join( sectionSeparator );
          resourceMgr()->setValue( section, param, value );
        }
      }
      else if ( sMod ) {
        // received message for the module
        QString msg = QStringList( data.mid(1) ).join( sectionSeparator );
        sMod->message( msg );
      }
    }
  }
}

void LightApp_Application::onInfoPanelShown()
{
  if ( activeModule() && activeModule()->inherits( "LightApp_Module" ) )
    ((LightApp_Module*)activeModule())->updateInfoPanel();
}

/*!
  Internal method.
  Returns all top level toolbars.
  Note : Result list contains only main window toolbars, not including toolbars from viewers.
*/
QList<QToolBar*> LightApp_Application::findToolBars( const QStringList& names )
{
  QList<QToolBar*> aResult;
  QList<QToolBar*> tbList = desktop()->findChildren<QToolBar*>();
  for ( QList<QToolBar*>::iterator tit = tbList.begin(); tit != tbList.end(); ++tit ) {
    QToolBar* tb = *tit;
    QObject* po = Qtx::findParent( tb, "QMainWindow" );
    if ( po != desktop() )
      continue;
    if ( names.isEmpty() || names.contains( tb->objectName() ) )
      aResult.append(tb);
  }
  return aResult;
}

/*!
  Internal method to parse toolbars and dockable windows state.
*/
QByteArray LightApp_Application::processState(QByteArray& input,
                                              const bool processWin,
                                              const bool processTb,
                                              const bool isRestoring,
                                              QByteArray defaultState) {

  QByteArray aRes;
  bool hasDefaultState  = !defaultState.isEmpty();
  bool isDockWinWriten = false;
  int nbDocWin = -1;
  //Write date from users settings
  if(isRestoring){
    QDataStream tmpInputData(&input, QIODevice::ReadOnly);
    int marker, version;
    uchar dockmarker;
    tmpInputData >> marker;
    tmpInputData >> version;
    tmpInputData >> dockmarker;
    tmpInputData >> nbDocWin;
  }
  if(processWin && processTb && !isRestoring) {
    aRes = input;
  } else if(!processWin && !processTb ) {
    if(hasDefaultState)
      aRes = defaultState;
  } else {
    QDataStream aData(&aRes, QIODevice::WriteOnly);
    QList<QToolBar*> aToolBars = findToolBars();

    QStringList aNames;
    for ( QList<QToolBar*>::iterator tit = aToolBars.begin(); tit != aToolBars.end(); ++tit ) {
      QToolBar* tb = *tit;
      aNames.append(tb->objectName());
    }

    int toolBarMarkerIndex = getToolbarMarkerIndex(input,aNames);
    if(toolBarMarkerIndex < 0)
      return aRes;
    QDataStream anInputData(&input, QIODevice::ReadOnly);

    int toolBarMarkerIndexDef = 0;
    if(hasDefaultState) {
      toolBarMarkerIndexDef = getToolbarMarkerIndex(defaultState, aNames);
      if(toolBarMarkerIndexDef < 0)
        return aRes;
    }
    QDataStream anInputDataDef(&defaultState, QIODevice::ReadOnly);

    QDataStream* aTargetData = 0;
    int          aTargetIndex = -1;

    QByteArray currentArr = desktop()->saveState();
    QDataStream anInputDataCur(&currentArr, QIODevice::ReadOnly);
    bool useInputData = !isRestoring || (isRestoring && nbDocWin > 0);
    if(processWin && useInputData) {
      aTargetData = &anInputData;
      aTargetIndex = toolBarMarkerIndex;
    } else {
      //Write date from default settings
      if(hasDefaultState) {
        aTargetData = &anInputDataDef;
        aTargetIndex = toolBarMarkerIndexDef;
      } else {
        //If no default state, write current snapshot of the dockable windows
        if(isRestoring) {
          aTargetData = &anInputDataCur;
          int toolBarMarkerIndexCur = getToolbarMarkerIndex(currentArr, aNames);
          aTargetIndex = toolBarMarkerIndexCur;
        }
      }
    }

    if(aTargetData && aTargetIndex >= 0 ) {
      aTargetData->device()->seek(0);
      while( aTargetData->device()->pos() < aTargetIndex ) {
        uchar ch;
        *aTargetData >> ch;
        aData<<ch;
      }
      isDockWinWriten = true;
    }

    aTargetData = 0;
    aTargetIndex = -1;

    if(processTb) {
      aTargetData = &anInputData;
      aTargetIndex = toolBarMarkerIndex;
    } else {
      if(hasDefaultState) {
        aTargetData = &anInputDataDef;
        aTargetIndex = toolBarMarkerIndexDef;
      }
    }

    if(aTargetData && aTargetIndex >= 0) {
      int index;
      if(!isDockWinWriten ) {
        //Write version marker
        int marker, version;
        aTargetData->device()->seek(0);
        *aTargetData >> marker;
        *aTargetData >> version;
        aData << marker;
        aData << version;
        aData << (uchar) QDockWidgetMarker;
        aData << (int) 0;
        int shift = 4*sizeof(int) + sizeof(QSize);
        index = aTargetIndex - shift;
      } else {
        index = aTargetIndex;
      }

      aTargetData->device()->seek(index);
      while(!aTargetData->atEnd()) {
        uchar ch;
        *aTargetData >> ch;
        aData << ch;
      }
    } else { // Not treat toolbars
      aData << (uchar) QToolBarMarker;
      aData << (int) 0; //Nb toolbars = 0
    }
  }
  return aRes;
}

/*!
  \brief Emits operationFinished signal.
  \param theModuleName the name of the module which perfomed the operation
  \param theOperationName the operation name
  \param theEntryList the list of the created objects entries
*/
void LightApp_Application::emitOperationFinished( const QString& theModuleName,
                                                  const QString& theOperationName,
                                                  const QStringList& theEntryList )
{
  emit operationFinished( theModuleName, theOperationName, theEntryList );
}

/*!
  Update visibility state of given objects
*/
void LightApp_Application::updateVisibilityState( DataObjectList& theList,
                                                  SUIT_ViewModel* theViewModel )
{
  if ( !theViewModel || theList.isEmpty() ) return;

  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(activeStudy());
  if ( !aStudy ) return;

  SALOME_View* aView = dynamic_cast<SALOME_View*>( theViewModel );

  for ( DataObjectList::iterator itr = theList.begin(); itr != theList.end(); ++itr ) {
    LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>(*itr);

    if ( !obj || aStudy->isComponent( obj->entry() ) )
      continue;

    QString mname = aStudy->componentDataType(obj->entry());
    LightApp_Displayer* aDisplayer = LightApp_Displayer::FindDisplayer(mname, false);
    if ( aDisplayer ) {
      Qtx::VisibilityState anObjState = Qtx::UnpresentableState;
      if ( aDisplayer->canBeDisplayed( obj->entry(), theViewModel->getType() ) ) {
        if ( aDisplayer->IsDisplayed( obj->entry(), aView ) )
          anObjState = Qtx::ShownState;
        else
          anObjState = Qtx::HiddenState;
      }
      aStudy->setVisibilityState( obj->entry(), anObjState );
    }
  }
}

/*!
  Update presentations of all displayed objects of theComponent in specified viewers
*/
void LightApp_Application::updatePresentations( const QString& theComponent,
                                                const QStringList& theViewManagerTypes )
{
  LightApp_Displayer* aDisplayer = LightApp_Displayer::FindDisplayer(theComponent, false);
  if ( aDisplayer ) {
    LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(activeStudy());
    DataObjectList aComps;
    bool isFound = false;
    aStudy->root()->children( aComps );
    DataObjectList::const_iterator aCompsIt = aComps.begin();
    for ( ; aCompsIt != aComps.end() && !isFound; aCompsIt++ ) {
      LightApp_DataObject* aComp = dynamic_cast<LightApp_DataObject*>( *aCompsIt );
      if ( aComp && aComp->componentDataType() ==  theComponent) {
        isFound = true;
        DataObjectList anObjs;
        aComp->children(anObjs, true);

        QList<SUIT_ViewManager*> aViewMgrs;
        QStringList::const_iterator itVMTypes = theViewManagerTypes.begin();
        for ( ; itVMTypes != theViewManagerTypes.end(); ++itVMTypes )
          viewManagers( *itVMTypes, aViewMgrs );

        DataObjectList::const_iterator itObjs = anObjs.begin();
        for ( ; itObjs != anObjs.end(); itObjs++ ) {
          LightApp_DataObject* anObj = dynamic_cast<LightApp_DataObject*>( *itObjs );
          QString anEntry = anObj->entry();

          QListIterator<SUIT_ViewManager*> itViewMgrs( aViewMgrs );
          while ( itViewMgrs.hasNext()) {
            SUIT_ViewModel* aVM = itViewMgrs.next()->getViewModel();
            if ( aVM ) {
              SALOME_View* aView = dynamic_cast<SALOME_View*>(aVM);
              if ( aView ) {
                bool isDisp = aDisplayer->IsDisplayed( anEntry, aView );
                aDisplayer->Erase( anEntry, true, false, aView );
                if ( isDisp ) {
                  aDisplayer->Display( anEntry, false, aView );
                }
              }
            }
          }
        }
      }
    }
  }
}

/*!
 * Called when window activated
 */
void LightApp_Application::onWindowActivated( SUIT_ViewWindow* theViewWindow )
{
  SUIT_DataBrowser* anOB = objectBrowser();
  if ( !anOB )
    return;
  SUIT_DataObject* rootObj = anOB->root();
  if ( !rootObj )
    return;

  DataObjectList listObj = rootObj->children( true );

  SUIT_ViewModel* vmod = 0;
  if ( SUIT_ViewManager* vman = theViewWindow->getViewManager() )
    vmod = vman->getViewModel();
  updateVisibilityState( listObj, vmod );
}

/*!
  Called then view manager removed
*/
void LightApp_Application::onViewManagerRemoved( SUIT_ViewManager* )
{
  ViewManagerList lst;
  viewManagers( lst );
  if ( lst.count() == 1) { // in case if closed last view window
    LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( activeStudy() );
    if ( aStudy )
      aStudy->setVisibilityStateForAll( Qtx::UnpresentableState );
  }
}

/*!
  Check existing document.
*/
bool LightApp_Application::checkExistingDoc( bool closeExistingDoc )
{
  bool result = true;
  if( activeStudy() ) {
    int answer = !activeStudy()->isModified() ? 1 :
                 SUIT_MessageBox::question( desktop(),
                                            tr( "APPCLOSE_CAPTION" ),
                                            tr( "STUDYCLOSE_DESCRIPTION" ),
                                            tr( "APPCLOSE_SAVE" ),
                                            tr( "APPCLOSE_CLOSE" ),
                                            tr( "APPCLOSE_CANCEL" ), 0 );
    if(answer == 0) {
      if ( activeStudy()->isSaved() ) {
        onSaveDoc();
                if (closeExistingDoc) {
                        closeDoc(false);
                }
      } else if ( onSaveAsDoc() ) {
         if (closeExistingDoc) {
           if( !closeDoc( false ) ) {
             result = false;
           }
        }
      } else {
        result = false;
      }
    }
    else if( answer == 1 ) {
      if (closeExistingDoc) {
        closeDoc( false );
      }
    } else if( answer == 2 ) {
      result = false;
    }
  }
  return result;
}

#ifndef DISABLE_PYCONSOLE

PyConsole_Interp* LightApp_Application::getPyInterp()
{
  static PyConsole_Interp* myInterp = 0;
  if ( !myInterp ) {
    myInterp = createPyInterp();
    myInterp->initialize();
  }
  return myInterp;
}

PyConsole_Interp* LightApp_Application::createPyInterp()
{
  return new PyConsole_Interp();
}

#endif // DISABLE_PYCONSOLE

void LightApp_Application::createHelpItems( const QString& modTitle )
{
  if ( modTitle.isEmpty() )
    return;

  QString userGuide = "User's Guide";
  QString devGuide = "Developer's Guide";

  int helpMenu = createMenu( tr( "MEN_DESK_HELP" ), -1, -1, 1000 );

  createMenu( userGuide, helpMenu, -1, 5 );
  createMenu( devGuide, helpMenu, -1, 5 );

  IMap <QString, QString> helpData;                                 // list of help files for the module
  QString helpSubMenu;                                              // help submenu name (empty if not needed)
  QString modName = moduleName( modTitle );                         // module name
  if ( modName.isEmpty() ) modName = modTitle;                      // for KERNEL and GUI
  QString rootDir = QString( "%1_ROOT_DIR" ).arg( modName );        // module root dir env variable
  QString modDir = Qtx::getenv( rootDir.toUtf8().constData() );     // module root dir path
  QString docSection;
  if ( resourceMgr()->hasValue( modName, "documentation" ) )
    docSection = resourceMgr()->stringValue( modName, "documentation" );
  else if ( resourceMgr()->hasSection( modName + "_documentation" ) )
    docSection = modName + "_documentation";
  if ( !docSection.isEmpty() )
  {
    helpSubMenu = resourceMgr()->stringValue( docSection, "sub_menu", "" );
    if ( helpSubMenu.contains( "%1" ) )
      helpSubMenu = helpSubMenu.arg( modTitle );
    foreach( QString paramName, resourceMgr()->parameters( docSection ) )
    {
      QString key = paramName.contains( "%1" ) ? paramName.arg( modTitle ) : paramName;
      QString helpItem = getHelpItem( docSection, paramName );
      if ( !helpItem.isEmpty() )
        helpData.insert( key, helpItem );
    }
  }

  if ( helpData.isEmpty() && !modDir.isEmpty() )
  {
    QStringList idxLst = QStringList() << modDir << "share" << "doc" << "salome" << "gui" << modName << "index.html";
    QString indexFile = idxLst.join( QDir::separator() );          // index file
    if ( QFile::exists( indexFile ) )
      helpData.insert( tr( "%1 module Users's Guide" ).arg( modTitle ), indexFile );
  }

  IMapConstIterator<QString, QString > fileIt;
  for ( fileIt = helpData.begin(); fileIt != helpData.end(); fileIt++ )
  {
    QString helpItemPath = fileIt.key();
    // remove all '//' occurances
    while ( helpItemPath.contains( "//" ) )
      helpItemPath.replace( "//", "" );
    // obtain submenus hierarchy if given
    QStringList smenus = helpItemPath.split( "/" );
    helpItemPath = smenus.takeLast();
    // workaround for User's Guide and Developer's Guide to avoid having single item in module's submenu.
    if ( helpItemPath == userGuide || helpItemPath == devGuide )
    {
      QString menuPath = smenus.join( "/" );
      QStringList allKeys = helpData.keys();
      QStringList total = allKeys.filter( QRegExp( QString( "^%1" ).arg( menuPath ) ) );
      if ( total.count() == 1 && smenus.count() > 0 )
        helpItemPath = smenus.takeLast();
    }
    QPixmap helpIcon = fileIt.value().startsWith( "http", Qt::CaseInsensitive ) ?
      resourceMgr()->loadPixmap( "STD", tr( "ICON_WWW" ), false ) :
      resourceMgr()->loadPixmap( "STD", tr( "ICON_HELP" ), false );
    QAction* a = createAction( -1, helpItemPath, helpIcon, helpItemPath, helpItemPath,
                               0, desktop(), false, this, SLOT( onHelpContentsModule() ) );
    a->setData( fileIt.value() );
    if ( !helpSubMenu.isEmpty() )
      smenus.prepend( helpSubMenu );
    // create sub-menus hierarchy
    int menuId = helpMenu;
    foreach ( QString subMenu, smenus )
      menuId = createMenu( subMenu, menuId, -1, 5 );
    createMenu( a, menuId, -1, ( menuId != helpMenu && ( helpItemPath == userGuide || helpItemPath == devGuide ) ) ? 0 : 5 );
    if ( !myHelpItems.contains( modName ) )
      myHelpItems[modName] = IdList();
    myHelpItems[modName].append( actionId( a ) );
  }
}

void LightApp_Application::removeHelpItems( const QString& modTitle )
{
  QString modName = moduleName( modTitle );
  if ( myHelpItems.contains( modName ) )
  {
    foreach( int id, myHelpItems[modName] )
      setMenuShown( id, false );
    myHelpItems.remove( modName );
  }
}
