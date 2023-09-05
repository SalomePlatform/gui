// Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
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

#include "CAM_Application.h"

#include "CAM_Study.h"
#include "CAM_Module.h"

#include <SUIT_Tools.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#include <KERNEL_version.h>
#include <GUI_version.h>

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QRegExp>
#include <QTextStream>
#include <QDateTime>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <cstdio>
#include <iostream>

#include <utilities.h>

namespace
{
class BusyLocker
{
public:
  BusyLocker( bool& busy ) : myPrev( busy ), myBusy( busy ) { myBusy = true; }
  ~BusyLocker() { myBusy = myPrev; }
private:
  bool  myPrev;
  bool& myBusy;
};
}

/*!
  \brief Create new instance of CAM_Application.
  \return new instance of CAM_Application class
*/
extern "C" CAM_EXPORT SUIT_Application* createApplication()
{
  return new CAM_Application();
}

/*!
  \class CAM_Application
  \brief Introduces an application class which provides modular architecture.

  This class defines multi-modular application configuration and behaviour.
  Each module (CAM_Module) can have own data model, document windows and
  viewers, etc.

  An application provides all necessary functionality for modules management,
  like
  - loading of modules
  - modules activation/deactivation
  - etc
*/

CAM_Application::ModuleInfoList CAM_Application::myInfoList;

/*!
  \brief Constructor.

  Read modules list (from command line or from resource file).
  If \a autoLoad parameter is \c true all the modules will be loaded
  immediately after application starting, otherwise each module will
  be loaded by demand (with activateModule()).

  \param autoLoad auto loading flag
*/
CAM_Application::CAM_Application( const bool autoLoad )
: STD_Application(),
  myModule( 0 ),
  myAutoLoad( autoLoad ),
  myBlocked( false )
{
  readModuleList();
}

/*!
  \brief Destructor.

  Does nothing currently.
*/
CAM_Application::~CAM_Application()
{
  for ( QList<CAM_Module*>::const_iterator it = myModules.begin(); it != myModules.end(); ++it )
    delete *it;
  myModules.clear();
}

/*!
  \brief Start an application.

  Load all modules, if "auto loading" flag has been set to \c true.

  \sa CAM_Application()
*/
void CAM_Application::start()
{
  // check modules
  for ( ModuleInfoList::iterator it = myInfoList.begin();
        it != myInfoList.end(); ++it )
  {
    if ( (*it).status == stUnknown )
      (*it).status = checkModule( (*it).title ) ? stReady : stInaccessible;
  }

  // auto-load modules
  if ( myAutoLoad )
    loadModules();

  STD_Application::start();
}

/*!
  \brief Get active module.
  \return active module or 0 if there are no any
*/
CAM_Module* CAM_Application::activeModule() const
{
  return myModule;
}

/*!
  \brief Get the module with specified name.
  \return module or 0 if not found
*/
CAM_Module* CAM_Application::module(  const QString& modName ) const
{
  CAM_Module* mod = 0;
  for ( QList<CAM_Module*>::const_iterator it = myModules.begin();
        it != myModules.end() && !mod; ++it )
    if ( (*it)->moduleName() == modName )
      mod = *it;
  return mod;
}

/*!
  \brief Get all loaded modules.
  \return list of modules
*/
CAM_Application::ModuleList CAM_Application::modules() const
{
  return myModules;
}

/*!
  \brief Get all loaded modules.
  \param returning list of modules
*/
void CAM_Application::modules( CAM_Application::ModuleList& out ) const
{
  out.clear();

  for ( QList<CAM_Module*>::const_iterator it = myModules.begin();
        it != myModules.end(); ++it )
    out.append( *it );
}

/*!
  \brief Get names of all modules.

  Get loaded modules names if \a loaded is \c true,
  otherwise get all avaiable modules names.

  \param lst output list of modules names
  \param loaded boolean flag, defines what modules names to return
*/
void CAM_Application::modules( QStringList& lst, const bool loaded ) const
{
  lst.clear();

  if ( loaded )
  {
    for ( QList<CAM_Module*>::const_iterator it = myModules.begin();
          it != myModules.end(); ++it )
      lst.append( (*it)->moduleName() );
  }
  else
  {
    for ( ModuleInfoList::const_iterator it = myInfoList.begin();
          it != myInfoList.end(); ++it )
      if ( (*it).status != stNoGui )
        lst.append( (*it).title );
  }
}

/*!
  \brief Add module \a mod to the modules list.

  Performes module initialization. Does nothing if the module
  is already added.

  \param mod module being added
  \sa CAM_Module::initialize()
*/
void CAM_Application::addModule( CAM_Module* mod )
{
  if ( !mod || myModules.contains( mod ) )
    return;

  mod->initialize( this );

  QMap<CAM_Module*, int> map;

  ModuleList newList;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin();
        it != myInfoList.end(); ++it )
  {
    if ( (*it).title == mod->moduleName() )
      newList.append( mod );
    else
    {
      CAM_Module* curMod = module( (*it).title );
      if ( curMod )
        newList.append( curMod );
    }
  }

  for ( QList<CAM_Module*>::const_iterator it = myModules.begin();
        it != myModules.end(); ++it )
  {
    if ( !newList.contains( *it ) )
      newList.append( *it );
  }

  if ( !newList.contains( mod ) )
      newList.append( mod );

  myModules = newList;

  moduleAdded( mod );
}

/*!
  \brief Load modules from the modules information list.

  If some module can not be loaded, an error message is shown.
*/
void CAM_Application::loadModules()
{
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end(); ++it )
  {
    CAM_Module* mod = loadModule( (*it).title );
    if ( mod )
      addModule( mod );
    else {
      QString wrn = tr( "Can not load module %1" ).arg( (*it).title );
      if ( desktop() && desktop()->isVisible() )
        SUIT_MessageBox::critical( desktop(), tr( "Loading modules" ), wrn );
      else
        qWarning( qPrintable( wrn ) );
    }
  }
}

/*!
  \brief Load module \a modName.

  The function prints warning message if:
  - modules information list is empty
  - modules information list does not include specified module info
  - module library can not be loaded by some reason

  \param modName module name
  \return module object pointer or 0 if module could not be loaded
*/
CAM_Module* CAM_Application::loadModule( const QString& modName, const bool showMsg )
{
  if ( myInfoList.isEmpty() )
  {
    qWarning( qPrintable( tr( "Modules configuration is not defined." ) ) );
    return 0;
  }

  QString libName = moduleLibrary( modName );
  if ( libName.isEmpty() )
  {
    qWarning( qPrintable( tr( "Information about module \"%1\" doesn't exist." ).arg( modName ) ) );
    return 0;
  }

  QString err;
  GET_MODULE_FUNC crtInst = 0;
  GET_VERSION_FUNC getVersion = 0;

#ifdef WIN32
#ifdef UNICODE
  LPTSTR str_libname = new TCHAR[libName.length() + 1];
  str_libname[libName.toWCharArray(str_libname)] = '\0';
#else
  QByteArray arr = libName.toLatin1();
  LPTSTR str_libname = arr.constData();
#endif
  HINSTANCE modLib = ::LoadLibrary( str_libname );
#ifdef UNICODE
  delete str_libname;
#endif
  if ( !modLib )
  {
    LPVOID lpMsgBuf;
    ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                     FORMAT_MESSAGE_IGNORE_INSERTS, 0, ::GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, 0 );
#ifdef UNICODE
	QString out_err = QString::fromWCharArray((LPTSTR)lpMsgBuf);
#else
	QString out_err = (LPTSTR)lpMsgBuf;
#endif
    err = QString( "Failed to load  %1. %2" ).arg( libName ).arg(out_err);
    ::LocalFree( lpMsgBuf );
  }
  else
  {
    crtInst = (GET_MODULE_FUNC)::GetProcAddress( modLib, GET_MODULE_NAME );
    if ( !crtInst )
    {
      LPVOID lpMsgBuf;
      ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS, 0, ::GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, 0 );
#ifdef UNICODE
	  QString out_err = QString::fromWCharArray((LPTSTR)lpMsgBuf);
#else
	  QString out_err = (LPTSTR)lpMsgBuf;
#endif

	  err = QString( "Failed to find  %1 function. %2" ).arg( GET_MODULE_NAME ).arg( out_err );
	  ::LocalFree( lpMsgBuf );
    }

    getVersion = (GET_VERSION_FUNC)::GetProcAddress( modLib, GET_VERSION_NAME );
  }
#else
  void* modLib = dlopen( libName.toUtf8(), RTLD_LAZY | RTLD_GLOBAL );
  if ( !modLib )
    err = QString( "Can not load library %1. %2" ).arg( libName ).arg( dlerror() );
  else
  {
    crtInst = (GET_MODULE_FUNC)dlsym( modLib, GET_MODULE_NAME );
    if ( !crtInst )
      err = QString( "Failed to find function %1. %2" ).arg( GET_MODULE_NAME ).arg( dlerror() );

    getVersion = (GET_VERSION_FUNC)dlsym( modLib, GET_VERSION_NAME );
  }
#endif

  CAM_Module* module = crtInst ? crtInst() : 0;
  if ( module )
  {
    module->setModuleName( modName );
    module->setName( moduleName( modName ) );
  }

  if ( !err.isEmpty() && showMsg ) {
    if ( desktop() && desktop()->isVisible() )
      SUIT_MessageBox::warning( desktop(), tr( "Error" ), err );
    else
      qWarning( qPrintable( err ) );
  }

  char* version = getVersion ? getVersion() : 0;

  if ( version ) {
    for ( ModuleInfoList::iterator it = myInfoList.begin(); it != myInfoList.end(); ++it ) {
      if ( (*it).title == modName ) {
        if( (*it).version.isEmpty() ) {
          (*it).version = QString(version);
        }
        break;
      }
    }
  }

  return module;
}

/*!
  \brief Activate module \a modName.
  \param modName module name
  \return \c true, if module is loaded and activated successfully and \c false otherwise
*/
bool CAM_Application::activateModule( const QString& modName )
{
  if ( (!modName.isEmpty() && !activeStudy()) || myBlocked )
    return false;

  // VSR 25/10/2011: prevent nested activation/deactivation
  // See issues 0021307, 0021373
  BusyLocker lock( myBlocked );

  QString name = modName;
  if ( !name.isEmpty() && !moduleTitle( modName ).isEmpty() )
    name = moduleTitle( modName );

  bool res = false;
  if ( !name.isEmpty() )
  {
    setProperty("activateModule", true);
    CAM_Module* mod = module( name );
    if ( !mod )
      mod = loadModule( name );
    addModule( mod );

    if ( mod )
      res = activateModule( mod );
    setProperty("activateModule", QVariant());
  }
  else
    res = activateModule( 0 );

  return res;
}

/*!
  \brief Activate module \a mod.

  Shows error message if module could not be activated in the current study.

  \param mod module object pointer
  \return \c true, if module is loaded and activated successfully and \c false otherwise
*/
bool CAM_Application::activateModule( CAM_Module* mod )
{
  if ( mod && !activeStudy() )
    return false;

  if ( myModule == mod )
    return true;

  if ( myModule )
  {
    if ( myModule->deactivateModule( activeStudy() ) )
    {
      logUserEvent( tr( "MODULE_DEACTIVATED" ).arg( myModule->moduleName() ) );
    }
    moduleDeactivated( myModule );
  }
  myModule = mod;

  if ( myModule )
  {
    // Connect the module to the active study
    myModule->connectToStudy( dynamic_cast<CAM_Study*>( activeStudy() ) );
    if ( myModule->activateModule( activeStudy() ) )
    {
      logUserEvent( tr( "MODULE_ACTIVATED" ).arg( myModule->moduleName() ) );
    }
    else
    {
      myModule->setMenuShown( false );
      myModule->setToolShown( false );
      QString wrn = tr( "ERROR_ACTIVATE_MODULE_MSG" ).arg( myModule->moduleName() );
      if ( desktop() && desktop()->isVisible() )
        SUIT_MessageBox::critical( desktop(), tr( "ERROR_TLT" ), wrn );
      else
        qWarning( qPrintable( wrn ) );
      myModule = 0;
      return false;
    }
  }

  updateCommandsStatus();

  return true;
}

/*!
  \brief Load module \a modName and activate its operation, corresponding to \a actionId.
  This method is dedicated to run operations of some module from any other module.
  \param modName module name
  \param actionId is a numerical unique operation identifier
  \return \c true in case of success and \c false otherwise
*/
bool CAM_Application::activateOperation( const QString& modName,
                                         const int actionId )
{
  CAM_Module* mod = loadModule(modName, false);
  if (mod) {
    addModule(mod);
    return mod->activateOperation(actionId);
  }
  return false;
}

/*!
  \brief Load module \a modName and activate its operation, corresponding to \a actionId.
  This method is dedicated to run operations of some module from any other module.
  \param modName module name
  \param actionId is a string unique operation identifier
  \return \c true in case of success and \c false otherwise
*/
bool CAM_Application::activateOperation( const QString& modName,
                                         const QString& actionId )
{
  CAM_Module* mod = loadModule(modName, false);
  if (mod) {
    addModule(mod);
    return mod->activateOperation(actionId);
  }
  return false;
}

/*!
  \brief Load module \a modName and activate its operation,
         corresponding to \a actionId and \a pluginName.
  This method is dedicated to run operations of some module from any other module.
  \param modName module name
  \param actionId is a string unique operation identifier
  \param pluginName is a name of a plugin where the operation is implemented
  \return \c true in case of success and \c false otherwise
*/
bool CAM_Application::activateOperation( const QString& modName,
                                         const QString& actionId,
                                         const QString& pluginName )
{
  CAM_Module* mod = loadModule(modName, false);
  if (mod) {
    addModule(mod);
    return mod->activateOperation(actionId, pluginName);
  }
  return false;
}

/*!
  \brief Create new study.
  \return study object pointer
*/
SUIT_Study* CAM_Application::createNewStudy()
{
  return new CAM_Study( this );
}

/*!
  \brief Update menu commands status.
*/
void CAM_Application::updateCommandsStatus()
{
  STD_Application::updateCommandsStatus();

  if ( activeModule() )
    activeModule()->updateCommandsStatus();
}

/*!
  \brief Prepare application to study closing.

  Closes all modules in study \a theDoc.

  \param theDoc study
*/
void CAM_Application::beforeCloseDoc( SUIT_Study* theDoc )
{
  for ( QList<CAM_Module*>::iterator it = myModules.begin(); it != myModules.end(); ++it )
    (*it)->studyClosed( theDoc );
}

void CAM_Application::afterCloseDoc()
{
}

/*!
  \brief Set active study.
  \param study study to be made active
*/
void CAM_Application::setActiveStudy( SUIT_Study* study )
{
  STD_Application::setActiveStudy( study );
}

/*!
  \brief Check module availability.

  The method can be redefined in successors. Default implementation returns \c true.

  \param title module title
  \return \c true if module is accessible; \c false otherwise
*/
bool CAM_Application::checkModule( const QString& )
{
  return true;
}

/*!
  \brief Callback function, called when the module is added to the application.

  This virtual method can be re-implemented in the successors. Base implementation
  does nothing.

  \param mod module being added
*/
void CAM_Application::moduleAdded( CAM_Module* /*mod*/ )
{
}

/*!
  \brief Callback function, called when the module is just deactivated.

  This virtual method can be re-implemented in the successors. Base implementation
  does nothing.

  \param mod module just deactivated
*/
void CAM_Application::moduleDeactivated( CAM_Module* /*mod*/ )
{
}

/*!
  \brief Get module name by its title (user name).
  \param title module title (user name)
  \return module name or null QString if module is not found
*/
QString CAM_Application::moduleName( const QString& title )
{
  QString res;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end() && res.isEmpty(); ++it )
  {
    if ( (*it).title == title )
      res = (*it).name;
  }
  return res;
}

/*!
  \brief Get module title (user name) by its name.
  \param name module name
  \return module title (user name) or null QString if module is not found
*/
QString CAM_Application::moduleTitle( const QString& name )
{
  QString res;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end() && res.isEmpty(); ++it )
  {
    if ( (*it).name == name )
      res = (*it).title;
  }
  return res;
}

/*!
  \brief Get module icon name.
  \param name module name or title
  \return module icon or null QString if module is not found
*/
QString CAM_Application::moduleIcon( const QString& name )
{
  QString res;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end() && res.isNull(); ++it )
  {
    if ( (*it).name == name || (*it).title == name )
      res = (*it).icon;
  }
  return res;
}

/*!
  \brief Get module description.
  \param name module name or title
  \return module description or null QString if description is not provided in config file.
*/
QString CAM_Application::moduleDescription( const QString& name )
{
  QString res;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end() && res.isNull(); ++it )
  {
    if ( (*it).name == name || (*it).title == name )
      res = tr((*it).description.toUtf8());
  }
  return res;
}

/*!
  \brief Get module library name by its title (user name).
  \param title module name or title
  \param full if \c true, return full library name, otherwise return its internal name
  \return module library name or null QString if module is not found
 */
QString CAM_Application::moduleLibrary( const QString& name, const bool full )
{
  QString res;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end() && res.isEmpty(); ++it )
  {
    if ( (*it).name == name || (*it).title == name )
      res = (*it).library;
  }
  if ( !res.isEmpty() && full )
    res = SUIT_Tools::library( res );
  return res;
}

/*!
  \brief Get displayer proxy for given module, by its title (user name).
  \param name module name or title
  \return name of module which provides displayer for requested module
 */
QString CAM_Application::moduleDisplayer( const QString& name )
{
  QString res;

  if ( !name.isEmpty() )
  {
    for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end() && res.isEmpty(); ++it )
    {
      if ( (*it).title == name || (*it).name == name ) {
        res = (*it).displayer;
        if ( res.isEmpty() )
          res = (*it).title;
      }
    }
  }

  return res;
}

/*!
  \brief Read modules information list

  This function first tries to get the modules names list by parsing
  the application command line arguments, looking for the
  "--modules ( <mod_name>[:<mod_name>...] )" option.
  List of modules is separated by colon symbol (":").

  If "--modules" command line option is not used, the list of modules
  is retrieved from the application resource file: parameter "modules" of
  the section "launch".

  Then the information about each module (module title (user name),
  library name) is retrieved from the corresponding section of resource
  file with help of resources manager.

  Shows the warning message, if module information list is empty.

  \sa SUIT_ResourceMgr
*/
void CAM_Application::readModuleList()
{
  if ( !myInfoList.isEmpty() )
    return;

  // we cannot use own resourceMgr() as this method can be called from constructor
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QStringList modList;

  QString args = QApplication::arguments().join( " " );

  QRegExp rx1("--modules=([\\w,]*)");
  rx1.setMinimal( false );
  QRegExp rx2("--modules\\s+\\(\\s*(.*)\\s*\\)");
  rx2.setMinimal( true );
  int pos = 0;
  while ( 1 ) {
    QString modules;
    int pos1 = rx1.indexIn( args, pos );
    int pos2 = rx2.indexIn( args, pos );
    if ( pos1 != -1 && pos2 != -1 ) {
      modules = pos1 < pos2 ? rx1.cap( 1 ) : rx2.cap(1);
      pos = pos1 < pos2 ? pos1 + rx1.matchedLength() : pos2 + rx2.matchedLength();
    }
    else if ( pos1 != -1 ) {
      modules = rx1.cap( 1 );
      pos = pos1 + rx1.matchedLength();
    }
    else if ( pos2 != -1 ) {
      modules = rx2.cap( 1 );
      pos = pos2 + rx2.matchedLength();
    }
    else {
      break;
    }

    modList.clear();
    QStringList mods = modules.split( QRegExp( "[:|,\\s]" ), QString::SkipEmptyParts );
    for ( int i = 0; i < mods.count(); i++ ) {
      if ( !mods[i].trimmed().isEmpty() )
        modList.append( mods[i].trimmed() );
    }
  }

  if ( modList.isEmpty() ) {
    QString mods = resMgr->stringValue( "launch", "modules", QString() );
    modList = mods.split( ",", QString::SkipEmptyParts );
  }

  // extra modules loaded manually on previous session
  // ...

  foreach ( QString modName, modList )
    appendModuleInfo( modName.trimmed() );

  if ( myInfoList.isEmpty() ) {
    if ( desktop() && desktop()->isVisible() )
      SUIT_MessageBox::warning( desktop(), tr( "Warning" ), tr( "Modules list is empty" ) );
    else
      {
        printf( "****************************************************************\n" );
        printf( "*    Warning: modules list is empty.\n" );
        printf( "****************************************************************\n" );
      }
  }
}

bool CAM_Application::appendModuleInfo( const QString& modName )
{
  MESSAGE("Start to append module info for a given module name: ");
  SCRUTE(modName.toStdString());

  if ( modName.isEmpty() )
    return false;  // empty module name

  if ( !moduleTitle( modName ).isEmpty() )
    return false;  // already added

  if ( modName == "KERNEL" || modName == "GUI" )
    return false; // skip KERNEL and GUI modules

  // we cannot use own resourceMgr() as this method can be called from constructor
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  ModuleInfo inf;

  // module internal name
  inf.name = modName;
  // module version
  inf.version = resMgr->stringValue( modName, "version", QString() ).trimmed();
  // displayer, if module does not have GUI, displayer may be delegated to other module
  inf.displayer = resMgr->stringValue( modName, "displayer", QString() ).trimmed();

  // "gui" option explicitly says that module has GUI
  // Now trying to get the "gui" option value, we always get a default one,
  // then we can't rely on it.
  bool hasGui = resMgr->booleanValue(modName, "gui", false);

  // Additional check if the module actually has a title and icon.
  // Module with GUI must explicitly specify title (GUI name).
  inf.title = resMgr->stringValue(modName, "name", QString()).trimmed();
  const bool hasTitle = !inf.title.isEmpty();
  SCRUTE(hasGui);
  SCRUTE(hasTitle);
  if (hasGui && !hasTitle)
  {
    MESSAGE("Invalid config! The module has gui option, but doesn't have a title.");
    return false;
  }

  // While we can't rely on gui option, use a title to make a decision about gui.
  hasGui = hasTitle;

  // status; if module has GUI, availability will be checked on activation
  inf.status = hasGui ? stUnknown : stNoGui;

  if ( hasGui )
  {
    // icon
    inf.icon = resMgr->stringValue( modName, "icon", QString() ).trimmed();
    // description, for Info panel
    inf.description = resMgr->stringValue( modName, "description", QString() );
    // library; if not specified, we use internal module name
    inf.library = SUIT_Tools::libraryName( resMgr->stringValue( modName, "library", QString() ).trimmed() );
    if ( inf.library.isEmpty() )
      inf.library = modName;
  }

  // At this point we should have only valid inf object.
  myInfoList.append(inf);

  SCRUTE(inf.name.toStdString());
  SCRUTE(inf.version.toStdString());
  SCRUTE(inf.displayer.toStdString());
  SCRUTE(inf.status);
  SCRUTE(inf.title.toStdString());
  SCRUTE(inf.icon.toStdString());
  SCRUTE(inf.description.toStdString());
  SCRUTE(inf.library.toStdString());

  return true;
}

void CAM_Application::removeModuleInfo( const QString& modName )
{
  QMutableListIterator<ModuleInfo> it( myInfoList );
  while ( it.hasNext() )
  {
    ModuleInfo info = it.next();
    if ( info.name == modName )
    {
      it.remove();
      break;
    }
  }
}

/*!
  \brief Add common menu items to the popup menu.

  Menu items list is defined by the active module.

  \param type popup menu context
  \param menu popup menu
  \param title popup menu title, which can be set by the module if required
*/
void CAM_Application::contextMenuPopup( const QString& type, QMenu* menu, QString& title )
{
  // to do : add common items for popup menu ( if they are exist )
  if ( activeModule() )
    activeModule()->contextMenuPopup( type, menu, title );
}

/*!
  \brief Create new empty study.
*/
void CAM_Application::createEmptyStudy()
{
  /*SUIT_Study* study = */activeStudy();
  STD_Application::createEmptyStudy();
}

/*!
  \brief Return information about version of the each module.
*/
CAM_Application::ModuleShortInfoList CAM_Application::getVersionInfo()
{
  ModuleShortInfoList info;

  ModuleShortInfo kernel;
  kernel.name = "KERNEL";
  kernel.version = KERNEL_VERSION_STR;
  info.append(kernel);

  ModuleShortInfo gui;
  gui.name = "GUI";
  gui.version = GUI_VERSION_STR;
  info.append(gui);

  for(int i = 0; i < myInfoList.size(); i++) {
    ModuleShortInfo infoItem;
    infoItem.name = myInfoList.at(i).title.isEmpty() ? myInfoList.at(i).name : myInfoList.at(i).title;
    infoItem.version = myInfoList.at(i).version;
    info.append(infoItem);
  }
  return info;
}

/*!
  \brief Abort active operations if there are any

  Iterates through all modules and asks each of them if there are pending operations that cannot be aborted.

  \return \c false if some operation cannot be aborted
*/
bool CAM_Application::abortAllOperations()
{
  bool aborted = true;
  for ( QList<CAM_Module*>::const_iterator it = myModules.begin(); it != myModules.end() && aborted; ++it )
  {
    aborted = (*it)->abortAllOperations();
  }
  return aborted;
}

/*!
  \brief Log GUI event.
  \param eventDescription GUI event description.
*/
void CAM_Application::logUserEvent( const QString& eventDescription )
{
  static QString guiLogFile; // null string means log file was not initialized yet
  static QMutex aGUILogMutex;

  if ( guiLogFile.isNull() )
  {
    // log file was not initialized yet, try to do that by parsing command line arguments
    guiLogFile = ""; // empty string means initialization was done but log file was not set
    QStringList args = QApplication::arguments();
    for ( int i = 1; i < args.count(); i++ )
    {
      QRegExp rxs ( "--gui-log-file=(.+)" );
      if ( rxs.indexIn( args[i] ) >= 0 && rxs.capturedTexts().count() > 1 )
      {
        QString file = rxs.capturedTexts()[1];
        QFileInfo fi ( file );
        if ( !fi.isDir() && fi.dir().exists() )
	{
	  guiLogFile = fi.absoluteFilePath();
	  if ( fi.exists() ) {
	    QFile file ( guiLogFile );
	    file.remove(); // remove probably existing log file, to start with empty one
          }
        }
        break;
      }
    }
  }
  if ( !guiLogFile.isEmpty() ) // non-empty string means log file was already initialized
  {
    QMutexLocker aLocker( &aGUILogMutex );
    QFile file ( guiLogFile );
    if ( file.open( QFile::Append ) ) // append to log file
    {
      QDateTime current = QDateTime::currentDateTime();
      QTextStream stream( &file );
      stream << current.toString("yyyyMMdd-hhmmss") << ": " << eventDescription << endl;
      file.close();
    }
  }
}

/*!
  \brief Log given action.
  \param action GUI action being logged.
  \param moduleName optional name of module, owning an action
*/
void CAM_Application::logAction( QAction* action, const QString& moduleName )
{
  QString text = action->toolTip();
  if ( text.isEmpty() )
    text = action->text();
  if ( text.isEmpty() )
    text = action->iconText();
  if ( !text.isEmpty() )
  {
    QStringList message;
    if ( !moduleName.isEmpty() )
      message << moduleName;
    if ( action->isCheckable() )
    {
      message << tr( "ACTION_TOGGLED" );
      message << ( action->isChecked() ? tr( "ACTION_ON" ) : tr( "ACTION_OFF" ) );
    }
    else
    {
      message << tr( "ACTION_TRIGGERED" );
    }
    message << text;
    logUserEvent( message.join( ": " ) );
  }
}
