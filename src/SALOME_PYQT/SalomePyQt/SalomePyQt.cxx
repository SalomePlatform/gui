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
// File   : SalomePyQt.cxx
// Author : Vadim SANDLER, Open CASCADE SAS, vadim.sandler@opencascade.com
//

#include <SALOME_PYQT_Module.h> // this include must be first!!!
#include "SalomePyQt.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QImage>
#include <QStringList>

#include <SALOME_Event.h>

#include <QtxAction.h>
#include <QtxActionMenuMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Tools.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <LogWindow.h>

/*!
  \brief Get the currently active application.
  \internal
  \return active application object or 0 if there is no any
*/
static SalomeApp_Application* getApplication()
{
  if ( SUIT_Session::session() )
    return dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  return 0;
}

/*!
  \brief Get the currently active study.
  \internal
  \return active study or 0 if there is no study opened
*/
static SalomeApp_Study* getActiveStudy()
{
  if ( getApplication() )
    return dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  return 0;
}

/*!
  \brief Get the currently active module.
  \internal
  This function returns correct result only if Python-based
  module is currently active. Otherwize, 0 is returned.
*/
static SALOME_PYQT_Module* getActiveModule()
{
  SALOME_PYQT_Module* module = 0;
  if ( SalomeApp_Application* anApp = getApplication() ) {
    module = SALOME_PYQT_Module::getInitModule();
    if ( !module )
      module = dynamic_cast<SALOME_PYQT_Module*>( anApp->activeModule() );
  }
  return module;
}

/*!
  \class SALOME_Selection
  \brief The class represents selection which can be used in Python.
*/

/*!
  \brief Map of created selection objects.
  \internal
*/
static QMap<SalomeApp_Application*, SALOME_Selection*> SelMap;

/*!
  \brief Get the selection object for the specified application.

  Finds or creates the selection object (one per study).

  \param app application object
  \return selection object or 0 if \a app is invalid
*/
SALOME_Selection* SALOME_Selection::GetSelection( SalomeApp_Application* app )
{
  SALOME_Selection* sel = 0;
  if ( app && SelMap.find( app ) != SelMap.end() )
    sel = SelMap[ app ];
  else 
    sel = SelMap[ app ] = new SALOME_Selection( app );
  return sel;
}

/*!
  \brief Constructor.
  \param p parent object
*/
SALOME_Selection::SALOME_Selection( QObject* p ) : QObject( p ), mySelMgr( 0 )
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( p );
  if ( app ) {
    mySelMgr = app->selectionMgr();
    connect( mySelMgr, SIGNAL( selectionChanged() ), this, SIGNAL( currentSelectionChanged() ) );
    connect( mySelMgr, SIGNAL( destroyed() ),        this, SLOT  ( onSelMgrDestroyed() ) );
  }
}
/*!
  \brief Destructor.
*/
SALOME_Selection::~SALOME_Selection()
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( parent() );
  if ( app && SelMap.find( app ) != SelMap.end() )
    SelMap.remove( app );
}

/*!
  \brief Called when selection manager is destroyed (usually 
  when the study is closed).
*/
void SALOME_Selection::onSelMgrDestroyed()
{
  mySelMgr = 0;
}

/*!
  \brief Clear the selection.
*/
void SALOME_Selection::Clear()
{
  class TEvent: public SALOME_Event {
    LightApp_SelectionMgr* mySelMgr;
  public:
    TEvent( LightApp_SelectionMgr* selMgr ) 
      : mySelMgr( selMgr ) {}
    virtual void Execute() {
      if ( mySelMgr )
        mySelMgr->clearSelected();
    }
  };
  ProcessVoidEvent( new TEvent( mySelMgr ) );
}

/*!
  \brief Clear the selection.
*/
void SALOME_Selection::ClearIObjects()
{
  Clear();
}

/*!
  Removes all selection filters.
*/
void SALOME_Selection::ClearFilters()
{
  class TEvent: public SALOME_Event {
    LightApp_SelectionMgr* mySelMgr;
  public:
    TEvent( LightApp_SelectionMgr* selMgr ) 
      : mySelMgr( selMgr ) {}
    virtual void Execute() {
      if ( mySelMgr )
        mySelMgr->clearFilters();
    }
  };
  ProcessVoidEvent( new TEvent( mySelMgr ) );
}

/*!
  \class SalomePyQt
  \brief The class provides utility functions which can be used in the Python
  to operate with the SALOME GUI.

  All the functionality of this class is implemented as static methods, so they
  can be called with the class name prefixed or via creation of the class instance.
  For example, next both ways of SalomePyQt class usage are legal:
  \code
  from SalomePyQt import *
  sg = SalomePyQt()
  # using SalomePyQt class instance
  desktop = sg.getDesktop()
  # using SalomePyQt class directly
  menubar = SalomePyQt.getMainMenuBar()
  \endcode
*/

/*!
  \fn QWidget* SalomePyQt::getDesktop();
  \brief Get the active application's desktop window.
  \return desktop window or 0 if there is no any
*/

class TGetDesktopEvent: public SALOME_Event 
{
public:
  typedef QWidget* TResult;
  TResult myResult;
  TGetDesktopEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    if ( getApplication() )
      myResult = (QWidget*)( getApplication()->desktop() );
  }
};
QWidget* SalomePyQt::getDesktop()
{
  return ProcessEvent( new TGetDesktopEvent() );
}

/*!
  \fn QWidget* SalomePyQt::getMainFrame();
  \brief Get current application's main frame widget [obsolete].

  Main frame widget is an internal widget of the application 
  desktop window (workspace).

  \return workspace widget (0 on any error)
*/

class TGetMainFrameEvent: public SALOME_Event
{
public:
  typedef QWidget* TResult;
  TResult myResult;
  TGetMainFrameEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    if ( getApplication() ) {
      SUIT_Desktop* aDesktop = getApplication()->desktop();
      myResult = (QWidget*)( aDesktop->centralWidget() );
    }
  }
};
QWidget* SalomePyQt::getMainFrame()
{
  return ProcessEvent( new TGetMainFrameEvent() );
}

/*!
  \fn QMenuBar* SalomePyQt::getMainMenuBar();
  \brief Get current application desktop's main menu.
  \return main menu object (0 on any error)
*/

class TGetMainMenuBarEvent: public SALOME_Event
{
public:
  typedef QMenuBar* TResult;
  TResult myResult;
  TGetMainMenuBarEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      myResult = anApp->desktop()->menuBar();
    }
  }
};
QMenuBar* SalomePyQt::getMainMenuBar() 
{
  return ProcessEvent( new TGetMainMenuBarEvent() );
}

/*!
  QMenu* SalomePyQt::getPopupMenu( const MenuName menu );
  \brief Get main menu's child popup submenu by its identifier.
  
  This function is obsolete. 
  Use QMenu* SalomePyQt::getPopupMenu( const QString& menu ) instead.

  \param menu menu identifier
  \return popup submenu object or 0 if it does not exist
*/

/*!
  QMenu* SalomePyQt::getPopupMenu( const QString& menu );
  \brief Get main menu's child popup submenu by its name.
  
  The function creates menu if it does not exist.

  \param menu menu name
  \return popup submenu object (0 on any error)
*/

class TGetPopupMenuEvent: public SALOME_Event
{
public:
  typedef QMenu* TResult;
  TResult myResult;
  QString myMenuName;
  TGetPopupMenuEvent( const QString& menu ) : myResult( 0 ), myMenuName( menu ) {}
  virtual void Execute()
  {
    SalomeApp_Application* anApp = getApplication();
    if ( anApp && !myMenuName.isEmpty() ) {
      QtxActionMenuMgr* mgr = anApp->desktop()->menuMgr();
      SALOME_PYQT_Module* module = getActiveModule();
      int id = -1;
      if ( module )
	id = module->createMenu( myMenuName, -1, -1, -1, -1 );
      else
	id = mgr->insert( myMenuName, -1, -1, -1, -1 );
      myResult = mgr->findMenu( id );
    }
  }
};
QMenu* SalomePyQt::getPopupMenu( const MenuName menu )
{
  QString menuName;
  switch( menu ) {
  case File:
    menuName = QApplication::translate( "", "MEN_DESK_FILE" );        break;
  case View:
    menuName = QApplication::translate( "", "MEN_DESK_VIEW" );        break;
  case Edit:
    menuName = QApplication::translate( "", "MEN_DESK_EDIT" );        break;
  case Preferences:
    menuName = QApplication::translate( "", "MEN_DESK_PREFERENCES" ); break;
  case Tools:
    menuName = QApplication::translate( "", "MEN_DESK_TOOLS" );       break;
  case Window:
    menuName = QApplication::translate( "", "MEN_DESK_WINDOW" );      break;
  case Help:
    menuName = QApplication::translate( "", "MEN_DESK_HELP" );        break;
  }
  return ProcessEvent( new TGetPopupMenuEvent( menuName ) );
}
QMenu* SalomePyQt::getPopupMenu( const QString& menu )
{
  return ProcessEvent( new TGetPopupMenuEvent( menu ) );
}

/*!
  \fn int SalomePyQt::getStudyId();
  \brief Get active study's identifier.
  \return active study ID or 0 if there is no active study
*/

class TGetStudyIdEvent: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  TGetStudyIdEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    if ( SalomeApp_Study* aStudy = getActiveStudy() ) {
      myResult = aStudy->studyDS()->StudyId();
    }
  }
};
int SalomePyQt::getStudyId()
{
  return ProcessEvent( new TGetStudyIdEvent() );
}

/*!
  \fn SALOME_Selection* SalomePyQt::getSelection()
  \brief Get the selection object for the current study.

  Creates a Selection object if it has not been created yet.

  \return selection object (0 on error)
*/

class TGetSelectionEvent: public SALOME_Event 
{
public:
  typedef SALOME_Selection* TResult;
  TResult myResult;
  TGetSelectionEvent() : myResult( 0 ) {}
  virtual void Execute() 
  {
    myResult = SALOME_Selection::GetSelection( getApplication() );
  }
};
SALOME_Selection* SalomePyQt::getSelection()
{
  return ProcessEvent( new TGetSelectionEvent() );
}

/*!
  \fn void SalomePyQt::putInfo( const QString& msg, const int sec )
  \brief Put an information message to the current application's 
  desktop status bar.

  Optional second delay parameter (\a sec) can be used to specify
  time of the message diplaying in seconds. If this parameter is less
  or equal to zero, the constant message will be put.

  \param msg message text 
  \param sec message displaying time in seconds
*/

class TPutInfoEvent: public SALOME_Event
{
  QString myMsg;
  int     mySecs;
public:
  TPutInfoEvent( const QString& msg, const int sec = 0 ) : myMsg( msg ), mySecs( sec ) {}
  virtual void Execute()
  {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      anApp->putInfo( myMsg, mySecs * 1000 );
    }
  }
};
void SalomePyQt::putInfo( const QString& msg, const int sec )
{
  ProcessVoidEvent( new TPutInfoEvent( msg, sec ) );
}

/*!
  \fn const QString SalomePyQt::getActiveComponent();
  \brief Get the currently active module name (for the current study).
  \return active module name or empty string if there is no active module
*/

class TGetActiveComponentEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  TGetActiveComponentEvent() {}
  virtual void Execute() 
  {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      if ( CAM_Module* mod = anApp->activeModule() ) {
        myResult = mod->name();
      }
    }
  }
};
const QString SalomePyQt::getActiveComponent()
{
  return ProcessEvent( new TGetActiveComponentEvent() );
}

/*!
  \brief Update an Object Browser of the specified (by identifier) study.

  If \a studyId <= 0 the active study's object browser is updated.
  The \a updateSelection parameter is obsolete and currently is not used. 
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  \brief studyId study identifier
  \brief updateSelection update selection flag (not used)
  \sa getActiveStudy()
*/
void SalomePyQt::updateObjBrowser( const int studyId, bool updateSelection )
{  
  class TEvent: public SALOME_Event
  {
    int  myStudyId;
    bool myUpdateSelection;
  public:
    TEvent( const int studyId, bool updateSelection ) 
      : myStudyId( studyId ), myUpdateSelection( updateSelection ) {}
    virtual void Execute()
    {
      if ( SUIT_Session::session() ) {
        if ( getActiveStudy() && myStudyId <= 0 )
          myStudyId = getActiveStudy()->id();
	if ( myStudyId > 0 ) {
          QList<SUIT_Application*> apps = SUIT_Session::session()->applications();
          QList<SUIT_Application*>::Iterator it;
	  for( it = apps.begin(); it != apps.end(); ++it ) {
            SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( *it );
            if ( anApp && anApp->activeStudy() && anApp->activeStudy()->id() == myStudyId ) {
	      anApp->updateObjectBrowser();
	      return;
	    }
          }
        }
      }
    }
  };
  ProcessVoidEvent( new TEvent( studyId, updateSelection ) );
}

/*!
  \brief Default resource file section name.
  \internal
*/
static const char* DEFAULT_SECTION = "SalomePyQt";

/*!
  \brief Add string setting to the application preferences.

  The parameter \a autoValue is obsolete parameter and currently is not used.
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  This function is obsolete. Use one of addSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \param value new setting value
  \param autoValue (not used)
*/
void SalomePyQt::addStringSetting( const QString& name, const QString& value, bool autoValue )
{
  class TEvent: public SALOME_Event
  {
    QString myName;
    QString myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const QString& value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute() {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	QStringList sl = myName.split( ":", QString::SkipEmptyParts );
	QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
	QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
	if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->setValue( _sec, _nam, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  \brief Add integer setting to the application preferences.

  The parameter \a autoValue is obsolete parameter and currently is not used.
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  This function is obsolete. Use one of addSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \param value new setting value
  \param autoValue (not used)
*/
void SalomePyQt::addIntSetting( const QString& name, const int value, bool autoValue)
{
  class TEvent: public SALOME_Event 
  {
    QString myName;
    int     myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const int value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute()
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	QStringList sl = myName.split( ":", QString::SkipEmptyParts );
	QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
	QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
	if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->setValue( _sec, _nam, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  \brief Add double setting to the application preferences.

  The parameter \a autoValue is obsolete parameter and currently is not used.
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  This function is obsolete. Use one of addSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \param value new setting value
  \param autoValue (not used)
*/
void SalomePyQt::addDoubleSetting( const QString& name, const double value, bool autoValue )
{
  class TEvent: public SALOME_Event 
  {
    QString myName;
    double  myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const double value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	QStringList sl = myName.split( ":", QString::SkipEmptyParts );
	QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
	QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
	if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->setValue( _sec, _nam, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  \brief Add boolean setting to the application preferences.

  The parameter \a autoValue is obsolete parameter and currently is not used.
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  This function is obsolete. Use one of addSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \param value new setting value
  \param autoValue (not used)
*/
void SalomePyQt::addBoolSetting( const QString& name, const bool value, bool autoValue )
{
  class TEvent: public SALOME_Event 
  {
    QString myName;
    bool    myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const bool value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	QStringList sl = myName.split( ":", QString::SkipEmptyParts );
	QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
	QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
	if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->setValue( _sec, _nam, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  \brief Remove setting from the application preferences.

  This function is obsolete. Use removeSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
*/
void SalomePyQt::removeSettings( const QString& name )
{
  class TEvent: public SALOME_Event {
    QString myName;
  public:
    TEvent( const QString& name ) : myName( name ) {}
    virtual void Execute() {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	QStringList sl = myName.split( ":", QString::SkipEmptyParts );
	QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
	QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
	if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->remove( _sec, _nam );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name ) );
}

/*!
  \fn QString SalomePyQt::getSetting( const QString& name );
  \brief Get application setting value (as string represenation).

  This function is obsolete. Use stringSetting(), integerSetting(), 
  boolSetting(), stringSetting() or colorSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \return setting name (empty string if setting name is invalid)
*/

class TGetSettingEvent: public SALOME_Event 
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myName;
  TGetSettingEvent( const QString& name ) : myName( name ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      QStringList sl = myName.split( ":", QString::SkipEmptyParts );
      QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
      QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
      myResult = ( !_sec.isEmpty() && !_nam.isEmpty() ) ? resMgr->stringValue( _sec, _nam, "" ) : QString( "" );
    }
  }
};
QString SalomePyQt::getSetting( const QString& name )
{
  return ProcessEvent( new TGetSettingEvent( name ) );
}

/*!
  \brief Add double setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const double value )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    double  myValue;
  public:
    TEvent( const QString& section, const QString& name, double value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \brief Add integer setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const int value )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    int     myValue;
  public:
    TEvent( const QString& section, const QString& name, int value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \brief Add boolean setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
  \param dumb this parameter is used in order to avoid sip compilation error 
  because of conflicting int and bool types
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const bool value, const int /*dumb*/ )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    bool    myValue;
  public:
    TEvent( const QString& section, const QString& name, bool value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \brief Add string setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const QString& value )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    QString myValue;
  public:
    TEvent( const QString& section, const QString& name, const QString& value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \brief Add color setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const QColor& value )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    QColor  myValue;
  public:
    TEvent( const QString& section, const QString& name, const QColor& value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \fn int SalomePyQt::integerSetting( const QString& section, 
                                      const QString& name, 
				      const int def );
  \brief Get integer setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetIntSettingEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetIntSettingEvent( const QString& section, const QString& name, const int def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->integerValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
int SalomePyQt::integerSetting( const QString& section, const QString& name, const int def )
{
  return ProcessEvent( new TGetIntSettingEvent( section, name, def ) );
}

/*!
  \fn double SalomePyQt::doubleSetting( const QString& section, 
                                        const QString& name, 
					const double def );
  \brief Get double setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetDblSettingEvent: public SALOME_Event 
{
public:
  typedef double TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetDblSettingEvent( const QString& section, const QString& name, const double def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->doubleValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
double SalomePyQt::doubleSetting( const QString& section, const QString& name, const double def )
{
  return ProcessEvent( new TGetDblSettingEvent( section, name, def ) );
}

/*!
  \fn bool SalomePyQt::boolSetting( const QString& section, 
                                    const QString& name, 
				    const bool def );
  \brief Get boolean setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetBoolSettingEvent: public SALOME_Event 
{
public:
  typedef bool TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetBoolSettingEvent( const QString& section, const QString& name, const bool def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->booleanValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
bool SalomePyQt::boolSetting( const QString& section, const QString& name, const bool def )
{
  return ProcessEvent( new TGetBoolSettingEvent( section, name, def ) );
}

/*!
  \fn QString SalomePyQt::stringSetting( const QString& section, 
                                         const QString& name, 
					 const QString& def );
  \brief Get string setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetStrSettingEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetStrSettingEvent( const QString& section, const QString& name, const QString& def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->stringValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
QString SalomePyQt::stringSetting( const QString& section, const QString& name, const QString& def )
{
  return ProcessEvent( new TGetStrSettingEvent( section, name, def ) );
}

/*!
  \fn QColor SalomePyQt::colorSetting( const QString& section, 
                                       const QString& name, 
				       const QColor def );
  \brief Get color setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetColorSettingEvent: public SALOME_Event 
{
public:
  typedef QColor TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetColorSettingEvent( const QString& section, const QString& name, const QColor& def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->colorValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
QColor SalomePyQt::colorSetting ( const QString& section, const QString& name, const QColor& def )
{
  return ProcessEvent( new TGetColorSettingEvent( section, name, def ) );
}

/*!
  \brief Remove setting from the application preferences.
  \param section resources file section name 
  \param name setting name
*/
void SalomePyQt::removeSetting( const QString& section, const QString& name )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
  public:
    TEvent( const QString& section, const QString& name ) : mySection( section ), myName( name ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->remove( mySection, myName );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name ) );
}

/*!
  \fn bool SalomePyQt::hasSetting( const QString& section, const QString& name );
  \brief Check setting existence in the application preferences.
  \param section resources file section name 
  \param name setting name
  \return \c true if setting exists
*/

class THasColorSettingEvent: public SALOME_Event 
{
public:
  typedef bool TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  THasColorSettingEvent( const QString& section, const QString& name ) 
    : mySection( section ), myName( name ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = resMgr->hasValue( mySection, myName );
    }
  }
};
bool SalomePyQt::hasSetting( const QString& section, const QString& name )
{
  return ProcessEvent( new THasColorSettingEvent( section, name ) );
}

/*!
  \fn QString SalomePyQt::getFileName( QWidget*           parent, 
				       const QString&     initial, 
				       const QStringList& filters, 
				       const QString&     caption,
				       bool               open );
  \brief Show 'Open/Save file' dialog box for file selection 
         and return a user's choice (selected file name).
  \param parent parent widget
  \param initial initial directory the dialog box to be opened in
  \param filters list of files filters (wildcards)
  \param caption dialog box title
  \param open if \c true, "Open File" dialog box is shown; 
         otherwise "Save File" dialog box is shown
  \return selected file name (null string if user cancels operation)
*/

class TGetFileNameEvent: public SALOME_Event 
{
public:
  typedef QString TResult;
  TResult     myResult;
  QWidget*    myParent;
  QString     myInitial;
  QStringList myFilters;
  QString     myCaption;
  bool        myOpen;
  TGetFileNameEvent( QWidget*           parent, 
		     const QString&     initial, 
		     const QStringList& filters, 
 		     const QString&     caption,
	 	     bool               open ) 
    : myParent ( parent ), 
      myInitial( initial ), 
      myFilters( filters ), 
      myCaption( caption ), 
      myOpen ( open ) {}
  virtual void Execute() 
  {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      myResult = anApp->getFileName( myOpen, myInitial, myFilters.join(";;"), 
				     myCaption, myParent );
    }
  }
};
QString SalomePyQt::getFileName( QWidget*           parent, 
				 const QString&     initial, 
				 const QStringList& filters, 
				 const QString&     caption,
				 bool               open )
{
  return ProcessEvent( new TGetFileNameEvent( parent, initial, filters, caption, open ) );
}

/*!
  \fn QStringList SalomePyQt::getOpenFileNames( QWidget*           parent, 
					        const QString&     initial, 
						const QStringList& filters, 
						const QString&     caption );
  \brief Show 'Open files' dialog box for multiple files selection
         and return a user's choice (selected file names list).
  \param parent parent widget
  \param initial initial directory the dialog box to be opened in
  \param filters list of files filters (wildcards)
  \param caption dialog box title
  \return selected file names list (empty list if user cancels operation)
*/

class TGetOpenFileNamesEvent: public SALOME_Event 
{
public:
  typedef QStringList TResult;
  TResult     myResult;
  QWidget*    myParent;
  QString     myInitial;
  QStringList myFilters;
  QString     myCaption;
  TGetOpenFileNamesEvent( QWidget*           parent, 
			  const QString&     initial, 
			  const QStringList& filters, 
			  const QString&     caption ) 
    : myParent ( parent ), 
      myInitial( initial ), 
      myFilters( filters ), 
      myCaption( caption ) {}
  virtual void Execute() 
  {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      myResult = anApp->getOpenFileNames( myInitial, myFilters.join(";;"), myCaption, myParent );
    }
  }
};
QStringList SalomePyQt::getOpenFileNames( QWidget*           parent, 
					  const QString&     initial, 
					  const QStringList& filters, 
					  const QString&     caption )
{
  return ProcessEvent( new TGetOpenFileNamesEvent( parent, initial, filters, caption ) );
}

/*!
  \fn QString SalomePyQt::getExistingDirectory( QWidget*       parent,
					        const QString& initial,
						const QString& caption );
  \brief Show 'Get Directory' dialog box for the directory selection
         and return a user's choice (selected directory name).
  \param parent parent widget
  \param initial initial directory the dialog box to be opened in
  \param caption dialog box title
  \return selected directory name (null string if user cancels operation)
*/

class TGetExistingDirectoryEvent: public SALOME_Event 
{
public:
  typedef QString TResult;
  TResult     myResult;
  QWidget*    myParent;
  QString     myInitial;
  QString     myCaption;
  TGetExistingDirectoryEvent( QWidget*           parent, 
			      const QString&     initial, 
			      const QString&     caption ) 
    : myParent ( parent ), 
      myInitial( initial ), 
      myCaption( caption ) {}
  virtual void Execute() 
  {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      myResult = anApp->getDirectory( myInitial, myCaption, myParent );
    }
  }
};
QString SalomePyQt::getExistingDirectory( QWidget*       parent,
					  const QString& initial,
					  const QString& caption )
{
  return ProcessEvent( new TGetExistingDirectoryEvent( parent, initial, caption ) );
}

/*!
  \brief Open external browser to display context help information.
  \todo

  Current implementation does nothing.

  \param source documentation (HTML) file name
  \param context context (for example, HTML ancor name)
*/
void SalomePyQt::helpContext( const QString& source, const QString& context ) 
{
  class TEvent: public SALOME_Event 
  {
    QString mySource;
    QString myContext;
  public:
    TEvent( const QString& source, const QString& context ) 
      : mySource( source ), myContext( context ) {}
    virtual void Execute() 
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
        anApp->onHelpContextModule( "", mySource, myContext );
      }
    }
  };
  ProcessVoidEvent( new TEvent( source, context ) );
}

/*!
  \fn bool SalomePyQt::dumpView( const QString& filename );
  \brief Dump the contents of the currently active view window 
  to the image file in the specified format.

  For the current moment JPEG, PNG and BMP images formats are supported.
  The image format is defined automatically by the file name extension.
  By default, BMP format is used.

  \param filename image file name
  \return operation status (\c true on success)
*/

class TDumpViewEvent: public SALOME_Event 
{
public:
  typedef bool TResult;
  TResult myResult;
  QString myFileName;
  TDumpViewEvent( const QString& filename ) 
    : myResult ( false ), myFileName( filename ) {}
  virtual void Execute() 
  {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      SUIT_ViewManager* vm = anApp->activeViewManager();
      if ( vm ) { 
        SUIT_ViewWindow* vw = vm->getActiveView();
	if ( vw ) {
          QImage im = vw->dumpView();
	  if ( !im.isNull() && !myFileName.isEmpty() ) {
            QString fmt = SUIT_Tools::extension( myFileName ).toUpper();
	    if ( fmt.isEmpty() ) fmt = QString( "BMP" ); // default format
	    if ( fmt == "JPG" )  fmt = "JPEG";
	    myResult = im.save( myFileName, fmt.toLatin1() );
          }
	}
      }
    }
  }
};
bool SalomePyQt::dumpView( const QString& filename )
{
  return ProcessEvent( new TDumpViewEvent( filename ) );
}

/*!
  \fn int SalomePyQt::defaultMenuGroup();
  \brief Get detault menu group identifier which can be used when 
  creating menus (insert custom menu commands).
  \return default menu group ID
*/

class TDefMenuGroupEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  TDefMenuGroupEvent() : myResult( -1 ) {}
  virtual void Execute() 
  {
    myResult = SALOME_PYQT_Module::defaultMenuGroup();
  }
};
int SalomePyQt::defaultMenuGroup()
{
  return ProcessEvent( new TDefMenuGroupEvent() );
}

class CrTool
{
public:
  CrTool( const QString& tBar ) 
    : myCase( 0 ), myTbName( tBar ) {}
  CrTool( const int id, const int tBar, const int idx ) 
    : myCase( 1 ), myId( id ), myTbId( tBar ), myIndex( idx ) {}
  CrTool( const int id, const QString& tBar, const int idx )
    : myCase( 2 ), myId( id ), myTbName( tBar ), myIndex( idx ) {}
  CrTool( QtxAction* action, const int tbId, const int id, const int idx )
    : myCase( 3 ), myAction( action ), myTbId( tbId ), myId( id ), myIndex( idx ) {}
  CrTool( QtxAction* action, const QString& tBar, const int id, const int idx )
    : myCase( 4 ), myAction( action ), myTbName( tBar ), myId( id ), myIndex( idx ) {}

  int execute( SALOME_PYQT_Module* module ) const
  {
    if ( module ) {
      switch ( myCase ) {
      case 0:
        return module->createTool( myTbName );
      case 1:
        return module->createTool( myId, myTbId, myIndex );
      case 2:
        return module->createTool( myId, myTbName, myIndex );
      case 3:
        return module->createTool( myAction, myTbId, myId, myIndex );
      case 4:
        return module->createTool( myAction, myTbName, myId, myIndex );
      }
    }
    return -1;
  }
private:
   int        myCase;
   QString    myTbName;
   int        myTbId;
   QtxAction* myAction;
   int        myId;
   int        myIndex;
};

class TCreateToolEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  const CrTool& myCrTool;
  TCreateToolEvent( const CrTool& crTool ) 
    : myResult( -1 ), myCrTool( crTool ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = myCrTool.execute( module );
  }
};

/*!
  \brief Create toolbar with specified name.
  \param tBar toolbar name
  \return toolbar ID or -1 if toolbar creation is failed
*/
int SalomePyQt::createTool( const QString& tBar )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( tBar ) ) );
}

/*! 
  \brief Insert action with specified \a id to the toolbar.
  \param id action ID
  \param tBar toolbar ID
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createTool( const int id, const int tBar, const int idx )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( id, tBar, idx ) ) );
}

/*!
  \brief Insert action with specified \a id to the toolbar.
  \param id action ID
  \param tBar toolbar name
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createTool( const int id, const QString& tBar, const int idx )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( id, tBar, idx ) ) );
}

/*!
  \brief Insert action to the toolbar.
  \param a action
  \param tBar toolbar ID
  \param id required action ID
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createTool( QtxAction* a, const int tBar, const int id, const int idx )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( a, tBar, id, idx ) ) );
}

/*!
  \brief Insert action to the toolbar.
  \param a action
  \param tBar toolbar name
  \param id required action ID
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createTool( QtxAction* a, const QString& tBar, const int id, const int idx )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( a, tBar, id, idx ) ) );
}

class CrMenu
{
public:
  CrMenu( const QString& subMenu, const int menu, const int id, const int group, const int idx ) 
    : myCase( 0 ), mySubMenuName( subMenu ), myMenuId( menu ), myId( id ), myGroup( group ), myIndex( idx ) {}
  CrMenu( const QString& subMenu, const QString& menu, const int id, const int group, const int idx ) 
    : myCase( 1 ), mySubMenuName( subMenu ), myMenuName( menu ), myId( id ), myGroup( group ), myIndex( idx ) {}
  CrMenu( const int id, const int menu, const int group, const int idx ) 
    : myCase( 2 ), myId( id ), myMenuId( menu ), myGroup( group ), myIndex( idx ) {}
  CrMenu( const int id, const QString& menu, const int group, const int idx ) 
    : myCase( 3 ), myId( id ), myMenuName( menu ), myGroup( group ), myIndex( idx ) {}
  CrMenu( QtxAction* action, const int menu, const int id, const int group, const int idx ) 
    : myCase( 4 ), myAction( action ), myMenuId( menu ), myId( id ), myGroup( group ), myIndex( idx ) {}
  CrMenu( QtxAction* action, const QString& menu, const int id, const int group, const int idx ) 
    : myCase( 5 ), myAction( action ), myMenuName( menu ), myId( id ), myGroup( group ), myIndex( idx ) {}

  int execute( SALOME_PYQT_Module* module ) const
  {
    if ( module ) {
      switch ( myCase ) {
      case 0:
        return module->createMenu( mySubMenuName, myMenuId, myId, myGroup, myIndex );
      case 1:
        return module->createMenu( mySubMenuName, myMenuName, myId, myGroup, myIndex );
      case 2:
        return module->createMenu( myId, myMenuId, myGroup, myIndex );
      case 3:
        return module->createMenu( myId, myMenuName, myGroup, myIndex );
      case 4:
        return module->createMenu( myAction, myMenuId, myId, myGroup, myIndex );
      case 5:
        return module->createMenu( myAction, myMenuName, myId, myGroup, myIndex );
      }
    }
    return -1;
  }
private:
   int        myCase;
   QString    myMenuName;
   int        myMenuId;
   QString    mySubMenuName;
   int        myGroup;
   QtxAction* myAction;
   int        myId;
   int        myIndex;
};

class TCreateMenuEvent: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  const CrMenu& myCrMenu;
  TCreateMenuEvent( const CrMenu& crMenu ) 
    : myResult( -1 ), myCrMenu( crMenu ) {}
  virtual void Execute()
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = myCrMenu.execute( module );
  }
};

/*!
  \brief Create main menu.
  \param subMenu menu name
  \param menu parent menu ID
  \param id required menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return menu ID or -1 if menu could not be added
*/
int SalomePyQt::createMenu( const QString& subMenu, const int menu, const int id, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( subMenu, menu, id, group, idx ) ) );
}

/*!
  \brief Create main menu.
  \param subMenu menu name
  \param menu parent menu name (list of menu names separated by "|")
  \param id required menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return menu ID or -1 if menu could not be added
*/
int SalomePyQt::createMenu( const QString& subMenu, const QString& menu, const int id, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( subMenu, menu, id, group, idx ) ) );
}

/*!
  \brief Insert action to the main menu.
  \param id action ID
  \param menu parent menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createMenu( const int id, const int menu, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( id, menu, group, idx ) ) );
}

/*!
  \brief Insert action to the main menu.
  \param id action ID
  \param menu parent menu name (list of menu names separated by "|")
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createMenu( const int id, const QString& menu, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( id, menu, group, idx ) ) );
}

/*!
  \brief Insert action to the main menu.
  \param a action
  \param menu parent menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createMenu( QtxAction* a, const int menu, const int id, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( a, menu, id, group, idx ) ) );
}

/*!
  \brief Insert action to the main menu.
  \param a action
  \param menu parent menu name (list of menu names separated by "|")
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createMenu( QtxAction* a, const QString& menu, const int id, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( a, menu, id, group, idx ) ) );
}

/*!
  \fn QtxAction* SalomePyQt::createSeparator();
  \brief Create separator action which can be used in the menu or toolbar.
  \return new separator action
*/

class TCreateSepEvent: public SALOME_Event 
{
public:
  typedef QtxAction* TResult;
  TResult myResult;
  TCreateSepEvent() 
    : myResult( 0 ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = (QtxAction*)module->separator();
  }
};
QtxAction* SalomePyQt::createSeparator()
{
  return ProcessEvent( new TCreateSepEvent() );
}

/*!
  \fn QtxAction* SalomePyQt::createAction( const int      id,
                                           const QString& menuText, 
					   const QString& tipText, 
					   const QString& statusText, 
					   const QString& icon,
					   const int      key, 
					   const bool     toggle )
  \brief Create an action which can be then used in the menu or toolbar.
  \param id the unique id action to be registered to
  \param menuText action text which should appear in menu
  \param tipText text which should appear in the tooltip
  \param statusText text which should appear in the status bar when action is activated
  \param icon the name of the icon file (the actual icon file name can be coded in the translation files)
  \param key the key accelrator for the action
  \param toggle if \c true the action is checkable
*/

class TCreateActionEvent: public SALOME_Event 
{
public:
  typedef QtxAction* TResult;
  TResult myResult;
  int     myId;
  QString myMenuText;
  QString myTipText;
  QString myStatusText;
  QString myIcon;
  int     myKey;
  bool    myToggle;
  TCreateActionEvent( const int id, const QString& menuText, const QString& tipText, 
		      const QString& statusText, const QString& icon, const int key, const bool toggle ) 
    : myResult( 0 ), myId( id ), myMenuText( menuText ), myTipText( tipText ),
      myStatusText( statusText ), myIcon( icon ), myKey( key ), myToggle( toggle ) {}
  virtual void Execute()
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = (QtxAction*)module->createAction( myId, myTipText, myIcon, myMenuText, myStatusText, myKey, myToggle );
  }
};
QtxAction* SalomePyQt::createAction( const int id,           const QString& menuText, 
				     const QString& tipText, const QString& statusText, 
				     const QString& icon,    const int key, const bool toggle )
{
  return ProcessEvent( new TCreateActionEvent( id, menuText, tipText, statusText, icon, key, toggle ) );
}

/*!
  \fn QtxAction* SalomePyQt::action( const int id )
  \brief Get action by specified identifier.
  \return action or 0 if action is not registered
*/

class TActionEvent: public SALOME_Event 
{
public:
  typedef QtxAction* TResult;
  TResult myResult;
  int     myId;
  TActionEvent( const int id )
    : myResult( 0 ), myId( id ) {}
  virtual void Execute()
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = (QtxAction*)module->action( myId );
  }
};
QtxAction* SalomePyQt::action( const int id )
{
  return ProcessEvent( new TActionEvent( id ) );
}

/*!
  \fn int SalomePyQt::actionId( const QtxAction* a );
  \brief Get an action identifier. 
  \return action ID or -1 if action is not registered
*/

class TActionIdEvent: public SALOME_Event 
{
public:
  typedef  int TResult;
  TResult  myResult;
  const QtxAction* myAction;
  TActionIdEvent( const QtxAction* action )
    : myResult( -1 ), myAction( action ) {}
  virtual void Execute()
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = module->actionId( myAction );
  }
};
int SalomePyQt::actionId( const QtxAction* a )
{
  return ProcessEvent( new TActionIdEvent( a ) );
}

/*!
  \fn int SalomePyQt::addGlobalPreference( const QString& label );
  \brief Add global (not module-related) preferences group.
  \param label global preferences group name
  \return preferences group identifier
*/

class TAddGlobalPrefEvent: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  QString myLabel;
  TAddGlobalPrefEvent( const QString& label )
    : myResult( -1 ), myLabel( label ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = module->addGlobalPreference( myLabel );
  }
};
int SalomePyQt::addGlobalPreference( const QString& label )
{
  return ProcessEvent( new TAddGlobalPrefEvent( label ) );
}

/*!
  \fn int SalomePyQt::addPreference( const QString& label );
  \brief Add module-related preferences group.
  \param label preferences group name
  \return preferences group identifier
*/

class TAddPrefEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  QString myLabel;
  TAddPrefEvent( const QString& label )
    : myResult( -1 ), myLabel( label ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = module->addPreference( myLabel );
  }
};
int SalomePyQt::addPreference( const QString& label )
{
  return ProcessEvent( new TAddPrefEvent( label ) );
}

/*!
  \fn int SalomePyQt::addPreference( const QString& label, const int pId, const int type,
			             const QString& section, const QString& param );
  \brief Add module-related preferences.
  \param label preferences group name
  \param pId parent preferences group id
  \param type preferences type
  \param section resources file section name
  \param param resources file setting name
  \return preferences identifier
*/

class TAddPrefParamEvent: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  QString myLabel;
  int     myPId;
  int     myType;
  QString mySection;
  QString myParam;
  TAddPrefParamEvent( const QString& label, 
		      const int pId, const int type,
		      const QString& section, 
		      const QString& param )
    : myResult( -1 ),
      myLabel( label ), myPId( pId ), myType( type ), 
      mySection( section ), myParam ( param ) {}
  virtual void Execute()
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = module->addPreference( myLabel, myPId, myType, mySection, myParam );
  }
};
int SalomePyQt::addPreference( const QString& label, const int pId, const int type,
			       const QString& section, const QString& param )
{
  return ProcessEvent( new TAddPrefParamEvent( label, pId, type, section, param ) );
}

/*!
  \fn QVariant SalomePyQt::preferenceProperty( const int id, const QString& prop );
  \brief Get the preferences property.
  \param id preferences identifier
  \param prop preferences property name
  \return preferences property value or null QVariant if property is not set
*/

class TPrefPropEvent: public SALOME_Event
{
public:
  typedef QVariant TResult;
  TResult myResult;
  int     myId;
  QString myProp;
  TPrefPropEvent( const int id, const QString& prop )
    : myId( id ), myProp( prop ) {}
  virtual void Execute()
  {
    SALOME_PYQT_Module* module = getActiveModule();
    if ( module )
      myResult = module->preferenceProperty( myId, myProp );
  }
};
QVariant SalomePyQt::preferenceProperty( const int id, const QString& prop )
{
  return ProcessEvent( new TPrefPropEvent( id, prop ) );
}

/*!
  \brief Set the preferences property.
  \param id preferences identifier
  \param prop preferences property name
  \param var preferences property value
*/
void SalomePyQt::setPreferenceProperty( const int id, 
					const QString& prop,
					const QVariant& var )
{
  class TEvent: public SALOME_Event
  {
    int      myId;
    QString  myProp;
    QVariant myVar;
  public:
    TEvent( const int id, const QString& prop, const QVariant& var ) 
      : myId( id ), myProp( prop ), myVar( var ) {}
    virtual void Execute() 
    {
      SALOME_PYQT_Module* module = getActiveModule();
      if ( module )
	module->setPreferenceProperty( myId, myProp, myVar );
    }
  };
  ProcessVoidEvent( new TEvent( id, prop, var) );
}

/*!
  \brief Add the property value to the list of values.

  This method allows creating properties which are QValueList<QVariant>
  - there is no way to pass such values directly to QVariant parameter with PyQt.

  \param id preferences identifier
  \param prop preferences property name
  \param idx preferences property index
  \param var preferences property value for the index \a idx
*/
void SalomePyQt::addPreferenceProperty( const int id, 
					const QString& prop,
					const int idx, 
					const QVariant& var )
{
  class TEvent: public SALOME_Event
  {
    int      myId;
    QString  myProp;
    int      myIdx;
    QVariant myVar;
  public:
    TEvent( const int id, const QString& prop, const int idx, const QVariant& var ) 
      : myId( id ), myProp( prop ), myIdx( idx), myVar( var ) {}
    virtual void Execute()
    {
      SALOME_PYQT_Module* module = getActiveModule();
      if ( module ) {
	QVariant var =  module->preferenceProperty( myId, myProp );
	if ( var.isValid() ) {
	  if ( var.type() == QVariant::StringList ) {
	    QStringList sl = var.toStringList();
	    if ( myIdx >= 0 && myIdx < sl.count() ) 
	      sl[myIdx] = myVar.toString();
	    else
	      sl.append( myVar.toString() );
	    module->setPreferenceProperty( myId, myProp, sl );
	  }
	  else if ( var.type() == QVariant::List ) {
	    QList<QVariant> vl = var.toList();
	    if ( myIdx >= 0 && myIdx < vl.count() ) 
	      vl[myIdx] = myVar;
	    else
	      vl.append( myVar );
	    module->setPreferenceProperty( myId, myProp, vl );
	  }
	}
	else {
	  QList<QVariant> vl;
	  vl.append( myVar );
	  module->setPreferenceProperty( myId, myProp, vl );
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( id, prop, idx, var) );
}

/*!
  \brief Put the message to the Log messages output window
  \param msg message text (it can be of simple rich text format)
  \param addSeparator boolean flag which specifies if it is necessary 
         to separate the message with predefined separator
*/
void SalomePyQt::message( const QString& msg, bool addSeparator )
{
  class TEvent: public SALOME_Event
  {
    QString  myMsg;
    bool     myAddSep;
  public:
    TEvent( const QString& msg, bool addSeparator ) 
      : myMsg( msg ), myAddSep( addSeparator ) {}
    virtual void Execute()
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	LogWindow* lw = anApp->logWindow();
	if ( lw )
	  lw->putMessage( myMsg, myAddSep );
      }
    }
  };
  ProcessVoidEvent( new TEvent( msg, addSeparator ) );
}

/*!
  \brief Remove all the messages from the Log messages output window.
*/
void SalomePyQt::clearMessages()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	LogWindow* lw = anApp->logWindow();
	if ( lw )
	  lw->clear();
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}
