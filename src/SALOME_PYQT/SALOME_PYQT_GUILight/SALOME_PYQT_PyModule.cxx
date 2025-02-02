// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

// File   : SALOME_PYQT_PyModule.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "SALOME_PYQT_PyModule.h"
#include "SALOME_PYQT_PyInterp.h"

#include "LightApp_Application.h"
#include "LightApp_DataObject.h"
#include "LightApp_Module.h"
#include "LightApp_Study.h"
#include "PyInterp_Dispatcher.h"
#include "QtxActionMenuMgr.h"
#include "QtxWorkspace.h"
#include "QtxWorkstack.h"
#include "STD_MDIDesktop.h"
#include "STD_TabDesktop.h"
#include "SUITApp_init_python.hxx"
#include "SUIT_ResourceMgr.h"
#include "SUIT_ViewManager.h"
#include "SUIT_ViewModel.h"
#include "SUIT_ViewWindow.h"

#include <QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QMenu>
#include <QMutex>

#include <utilities.h>

#include "sipAPISalomePyQtGUILight.h"

/*!
  \brief Default menu group number.
  \internal
*/
const int DEFAULT_GROUP = 40;

/*!
  \brief Mutex used to lock access from several threads to the shared
  (static) data
  \internal
*/
QMutex myInitMutex;

/*! DEBUG mode */
const bool theDEBUG = false;

/*!
  \var IsCallOldMethods
  \brief Allow calling obsolete callback methods.
  \internal
  
  If the macro CALL_OLD_METHODS is not defined, the invoking
  of obsolete Python module's methods like setSetting(), definePopup(), 
  etc. is blocked.

  CALL_OLD_METHODS macro can be defined, for example, by adding 
  -DCALL_OLD_METHODS compilation option to the CMakeLists.txt.
*/
#ifdef CALL_OLD_METHODS
const bool IsCallOldMethods = true;
#else
const bool IsCallOldMethods = false;
#endif

/*!
  \brief Get tag name for the DOM element.
  \internal
  \param element DOM element
  \return tag name or empty string if the element does not have tag name
*/
static QString tagName( const QDomElement& element )
{
  return element.tagName().trimmed();
}

/*!
  \brief Get value of DOM element's attribute.
  \internal
  \param element DOM element
  \param attName attribute name
  \return attribute value or empty string if the element does not have such attribute
*/
static QString attribute( const QDomElement& element, const QString& attName )
{
  return element.attribute( attName ).trimmed();
}

/*!
  \brief Inspect specified string for the boolean value.
  \internal
  
  This function returns \c true if string represents boolean value: 
  - "true", "yes" or "1" for \c true
  - "false", "no" or "0" for \c false
  Second parameter allows to specify what boolean value is expected:
  - 1: \c true
  - 0: \c false
  - other value is not taken into account (return represented value)

  \param value inspected string
  \param check expected boolean value
  \return boolean value represented by the string (\a check is not 1 or 0)
          or \c true if value correspond to the specified \a check
*/
static bool checkBool( const QString& value, const int check = -1 )
{
  QString v = value.toLower();
  if ( ( v == "true"  || v == "yes" || v == "1" ) && ( check != 0 ) )
    return true;
  if ( ( v == "false" || v == "no"  || v == "0" ) && ( check == 0 ) )
    return true;
  return false;
}

/*!
  \brief Inspect specified string for the integer value.
  \internal
  
  This function returns returns -1 if item is empty or represents
  an invalid number.
  \param value inspected string
  \param def default value
  \param shift shift value (it is added to the integer value to produce shifted result)
*/
static int checkInt( const QString& value, const int def = -1, const int shift = -1 )
{
  bool bOk;
  int val = value.toInt( &bOk );
  if ( !bOk ) val = def;
  if ( shift > 0 && bOk && val < 0 )
    val += shift;
  return val;
}

/*!
  \class FuncMsg
  \brief Function call in/out tracer.
  \internal
*/

class FuncMsg
{
public:
  FuncMsg( const QString& funcName )
  {
    myName = funcName;
    if ( theDEBUG )
      MESSAGE( qPrintable( myName ) << " [ begin ]" );
  }
  ~FuncMsg()
  {
    if ( theDEBUG )
      MESSAGE( qPrintable( myName ) << " [ end ]" );
  }
  void message( const QString& msg )
  {
    (void)msg; // unused in debug mode
    if ( theDEBUG )
      MESSAGE( qPrintable( myName ) << " : " << qPrintable( msg ) );
  }
private:
  QString myName;
};

/*!
  \class PyModuleHelper::InitLocker
  \brief Initialization locker
  \internal
*/

class PyModuleHelper::InitLocker
{
public:
  InitLocker( LightApp_Module* );
  ~InitLocker();
};

/*!
  \brief Constructor
  \internal
*/
PyModuleHelper::InitLocker::InitLocker( LightApp_Module* module )
{
  QMutexLocker ml( &myInitMutex );
  myInitModule = module;
}

/*!
  \brief Destructor
  \internal
*/
PyModuleHelper::InitLocker::~InitLocker()
{
  QMutexLocker ml( &myInitMutex );
  myInitModule = 0;
}

/*!
  \class PyModuleHelper::XmlHandler
  \brief XML resource files parser.
  \internal

  This class is used to provide backward compatibility with
  existing Python modules in which obsolete menu definition system
  (via XML files) is used.
*/

class PyModuleHelper::XmlHandler
{
public:
  XmlHandler( PyModuleHelper* helper, const QString& fileName );
  void             createActions();
  void             createPopup( QMenu* menu,
                                const QString& context,
                                const QString& parent,
                                const QString& object );
  void             activateMenus( bool );

private:
  LightApp_Module* module() const;
  QIcon            loadIcon( const QString& fileName );

  void             createMenu( QDomNode& parentNode,
                               const int parentMenuId = -1,
                               QMenu* parentPopup = 0 );
  void             createToolBar( QDomNode& parentNode );
  void             insertPopupItems( QDomNode& parentNode,
                                     QMenu* menu );

private:
  PyModuleHelper*  myHelper;
  QDomDocument     myDoc;
  QList<int>       myMenuItems;
};


/*!
  \brief Constructor
  \internal
  \param module pointer to the GUI module
  \param fileName path to the XML menu description file 
*/
PyModuleHelper::XmlHandler::XmlHandler( PyModuleHelper*  helper,
                                        const QString&   fileName )
: myHelper( helper )
{
  if ( !fileName.isEmpty() ) { 
    QFile aFile( fileName );
    if ( aFile.open( QIODevice::ReadOnly ) ) {
      myDoc.setContent( &aFile );
    }
  }
}

/*!
  \brief Parse XML file and create actions.
  \internal
  
  Called by PyModuleHelper::initialize() in order to create actions
  (menus, toolbars).
*/
void PyModuleHelper::XmlHandler::createActions()
{
  // get document element
  QDomElement aDocElem = myDoc.documentElement();

  // create main menu actions
  QDomNodeList aMenuList = aDocElem.elementsByTagName( "menu-item" );
  for ( int i = 0; i < aMenuList.count(); i++ ) {
    QDomNode n = aMenuList.item( i );
    createMenu( n );
  }

  // create toolbars actions
  QDomNodeList aToolsList = aDocElem.elementsByTagName( "toolbar" );
  for ( int i = 0; i < aToolsList.count(); i++ ) {
    QDomNode n = aToolsList.item( i );
    createToolBar( n );
  }
}

/*!
  \brief Create popup menu.
  \internal
  \param menu popup menu
  \param context popup menu context
  \param context popup menu parent object name
  \param context popup menu object name
*/
void PyModuleHelper::XmlHandler::createPopup( QMenu*         menu,
                                              const QString& context,
                                              const QString& parent,
                                              const QString& object )
{
  // get document element
  QDomElement aDocElem = myDoc.documentElement();

  // get popup menus actions
  QDomNodeList aPopupList = aDocElem.elementsByTagName( "popupmenu" );
  for ( int i = 0; i < aPopupList.count(); i++ ) {
    QDomNode n = aPopupList.item( i );
    if ( !n.isNull() && n.isElement() ) {
      QDomElement e = n.toElement();
      // QString lab = attribute( e, "label-id" ); // not used // 
      QString ctx = attribute( e, "context-id" );
      QString prt = attribute( e, "parent-id"  );
      QString obj = attribute( e, "object-id"  );
      if ( ctx == context && prt == parent && obj == object )  {
        insertPopupItems( n, menu );
        break;
      }
    }
  }
}

/*!
  \brief Activate/deactivate menus
  \internal
  \param enable if \c true menus are activated, otherwise menus are deactivated
*/
void PyModuleHelper::XmlHandler::activateMenus( bool enable )
{
  if ( module() ) {
    QtxActionMenuMgr* mgr = module()->menuMgr();
    foreach( int id, myMenuItems ) mgr->setEmptyEnabled( id, enable );
  }
}

/*!
  \brief Get owner module
*/
LightApp_Module* PyModuleHelper::XmlHandler::module() const
{
  return myHelper->module();
}

/*!
  \brief Load an icon from the module resources by the specified file name.
  \param fileName icon file name
  \return icon object
*/

QIcon PyModuleHelper::XmlHandler::loadIcon( const QString& fileName )
{
  QIcon icon;

  if ( module() && !fileName.isEmpty() ) {
      SUIT_ResourceMgr* resMgr = module()->getApp()->resourceMgr();
      QPixmap pixmap = resMgr->loadPixmap( module()->name(),
          QApplication::translate( module()->name().toLatin1().data(),
                                   fileName.toUtf8().data() ) );
      if ( !pixmap.isNull() )
        icon = QIcon( pixmap );
  }

  return icon;
}

/*!
  \brief Create main menu item and insert actions to it.
  \internal
  \param parentNode XML node with menu description
  \param parentMenuId parent menu ID (-1 for top-level menu)
  \param parentPopup parent popup menu (0 for top-level menu)
*/
void PyModuleHelper::XmlHandler::createMenu( QDomNode& parentNode, 
                                             const int parentMenuId,
                                             QMenu*    /*parentPopup*/ )
{
  if ( !module() || parentNode.isNull() )
    return;
  
  QDomElement parentElement = parentNode.toElement();
  if ( !parentElement.isNull() ) {
    QString plabel = attribute( parentElement, "label-id" );
    int     pid    = checkInt( attribute( parentElement, "item-id" ) );
    int     ppos   = checkInt( attribute( parentElement, "pos-id" ) );
    int     group  = checkInt( attribute( parentElement, "group-id" ), 
                               PyModuleHelper::defaultMenuGroup() );
    if ( !plabel.isEmpty() ) {
      QMenu* popup = 0;
      int menuId = -1;
      // create menu
      menuId = module()->createMenu( plabel,         // label
                                     parentMenuId,   // parent menu ID, -1 for top-level menu
                                     pid,            // ID
                                     group,          // group ID
                                     ppos );         // position
      myMenuItems.append( menuId );
      QDomNode node = parentNode.firstChild();
      while ( !node.isNull() ) {
        if ( node.isElement() ) {
          QDomElement elem = node.toElement();
          QString aTagName = tagName( elem );
          if ( aTagName == "popup-item" ) {
            int     id      = checkInt( attribute( elem, "item-id" ) );
            int     pos     = checkInt( attribute( elem, "pos-id" ) );
            int     group   = checkInt( attribute( elem, "group-id" ), 
                                        PyModuleHelper::defaultMenuGroup() );
            QString label   = attribute( elem, "label-id" );
            QIcon   icon    = loadIcon( attribute( elem, "icon-id" ) );
            QString tooltip = attribute( elem, "tooltip-id" );
            QString accel   = attribute( elem, "accel-id" );
            bool    toggle  = checkBool( attribute( elem, "toggle-id" ) );

            // -1 action ID is not allowed : it means that <item-id> attribute is missed in the XML file!
            // also check if the action with given ID is already created
            if ( id != -1 ) {
              // create menu action
              QAction* action = module()->createAction( id,                     // ID
                                                        tooltip,                // tooltip
                                                        icon,                   // icon
                                                        label,                  // menu text
                                                        tooltip,                // status-bar text
                                                        QKeySequence( accel ),  // keyboard accelerator
                                                        module(),               // action owner
                                                        toggle );               // toogled action
              myHelper->connectAction( action );
              module()->createMenu( action,   // action
                                    menuId,   // parent menu ID
                                    id,       // ID (same as for createAction())
                                    group,    // group ID
                                    pos );    // position
            }
          }
          else if ( aTagName == "submenu" ) {
            // create sub-menu
            createMenu( node, menuId, popup );
          }
          else if ( aTagName == "separator" ) {
            // create menu separator
            int id    = checkInt( attribute( elem, "item-id" ) ); // separator can have ID
            int pos   = checkInt( attribute( elem, "pos-id" ) );
            int group = checkInt( attribute( elem, "group-id" ), 
                                  PyModuleHelper::defaultMenuGroup() );
            QAction* action = module()->separator();
            module()->createMenu( action,  // separator action
                                  menuId,  // parent menu ID
                                  id,      // ID
                                  group,   // group ID
                                  pos );   // position
          }
        }
        node = node.nextSibling();
      }
    }
  }
}

/*!
  \brief Create a toolbar and insert actions to it.
  \param parentNode XML node with toolbar description
*/
void PyModuleHelper::XmlHandler::createToolBar( QDomNode& parentNode )
{
  if ( !module() || parentNode.isNull() )
    return;

  QDomElement parentElement = parentNode.toElement();
  if ( !parentElement.isNull() ) {
    QString aLabel = attribute( parentElement, "label-id" );
    QString aName  = attribute( parentElement, "name-id" );
    if ( !aLabel.isEmpty() ) {
      // create toolbar
      int tbId = module()->createTool( aLabel, aName );
      QDomNode node = parentNode.firstChild();
      while ( !node.isNull() ) {
        if ( node.isElement() ) {
          QDomElement elem = node.toElement();
          QString aTagName = tagName( elem );
          if ( aTagName == "toolbutton-item" ) {
            int     id      = checkInt( attribute( elem, "item-id" ) );
            int     pos     = checkInt( attribute( elem, "pos-id" ) );
            QString label   = attribute( elem, "label-id" );
            QIcon   icon    = loadIcon( attribute( elem, "icon-id" ) );
            QString tooltip = attribute( elem, "tooltip-id" );
            QString accel   = attribute( elem, "accel-id" );
            bool    toggle  = checkBool( attribute( elem, "toggle-id" ) );

            // -1 action ID is not allowed : it means that <item-id> attribute is missed in the XML file!
            // also check if the action with given ID is already created
            if ( id != -1 ) {
                // create toolbar action
                QAction* action = module()->createAction( id,                     // ID
                                                          tooltip,                // tooltip
                                                          icon,                   // icon
                                                          label,                  // menu text
                                                          tooltip,                // status-bar text
                                                          QKeySequence( accel ),  // keyboard accelerator
                                                          module(),               // action owner
                                                          toggle );               // toogled action
                myHelper->connectAction( action );
                module()->createTool( action, tbId, -1, pos );
            }
          }
          else if ( aTagName == "separatorTB" || aTagName == "separator" ) {
            // create toolbar separator
            int pos = checkInt( attribute( elem, "pos-id" ) );
            QAction* action = module()->separator();
            module()->createTool( action, tbId, -1, pos );
          }
        }
        node = node.nextSibling();
      }
    }
  }
}

/*!
  \brief Fill popup menu with the items.
  \param parentNode XML node with popup menu description
  \param menu popup menu
*/
void PyModuleHelper::XmlHandler::insertPopupItems( QDomNode& parentNode, QMenu* menu )
{
  if ( !module() && parentNode.isNull() )
    return;

  // we create popup menus without help of QtxPopupMgr
  QDomNode node = parentNode.firstChild();
  while ( !node.isNull() ) { 
    if ( node.isElement() ) {
      QDomElement elem = node.toElement();
      QString aTagName = tagName( elem );
      QList<QAction*> actions = menu->actions();
      if ( aTagName == "popup-item" ) {
        // insert a command item
        int     id      = checkInt( attribute( elem, "item-id" ) );
        int     pos     = checkInt( attribute( elem, "pos-id" ) );
        QString label   = attribute( elem, "label-id" );
        QIcon   icon    = loadIcon( attribute( elem, "icon-id" ) );
        QString tooltip = attribute( elem, "tooltip-id" );
        QString accel   = attribute( elem, "accel-id" );
        bool    toggle  = checkBool( attribute( elem, "toggle-id" ) );

        // -1 action ID is not allowed : it means that <item-id> attribute is missed in the XML file!
        // also check if the action with given ID is already created
        if ( id != -1 ) {
          QAction* action = module()->createAction( id,                     // ID
                                                    tooltip,                // tooltip
                                                    icon,                   // icon
                                                    label,                  // menu text
                                                    tooltip,                // status-bar text
                                                    QKeySequence( accel ),  // keyboard accelerator
                                                    module(),               // action owner
                                                    toggle );               // toogled action
          myHelper->connectAction( action );
          QAction* before = ( pos >= 0 && pos < actions.count() ) ? actions[ pos ] : 0;
          menu->insertAction( before, action );
        }
      }
      else if ( aTagName == "submenu" ) {
        // create sub-menu
        ////int     id    = checkInt( attribute( elem, "item-id" ) ); // not used //
        int     pos   = checkInt( attribute( elem, "pos-id" ) );
        QString label = attribute( elem, "label-id" );
        QString icon  = attribute( elem, "icon-id" );

        QIcon anIcon;
        if ( !icon.isEmpty() ) {
          QPixmap pixmap  = module()->getApp()->resourceMgr()->loadPixmap( module()->name(), icon );
          if ( !pixmap.isNull() )
            anIcon = QIcon( pixmap );
        }

        QMenu* newPopup = menu->addMenu( anIcon, label );
        QAction* before = ( pos >= 0 && pos < actions.count() ) ? actions[ pos ] : 0;
        menu->insertMenu( before, newPopup );
        insertPopupItems( node, newPopup );
      }
      else if ( aTagName == "separator" ) {
        // create menu separator
        int pos = checkInt( attribute( elem, "pos-id" ) );
        QAction* action = module()->separator();
        QAction* before = ( pos >= 0 && pos < actions.count() ) ? actions[ pos ] : 0;
        menu->insertAction( before, action );
      }
    }
    node = node.nextSibling();
  }
}

/*!
  \class PyModuleHelper
  \brief This class implements API helper for all the Python-based 
  SALOME GUI modules.
*/

LightApp_Module*          PyModuleHelper::myInitModule = 0;

/*!
  \brief Constructor
  \param module owner module
*/
PyModuleHelper::PyModuleHelper( LightApp_Module* module ) :
  QObject( module ),
  myModule( module ),
  myPyModule( 0 ), 
  myInterp( 0 ),
  myXmlHandler ( 0 ),
  myLastActivateStatus( true )
{
  setObjectName( "python_module_helper" );
}

/*!
  \brief Destructor
*/
PyModuleHelper::~PyModuleHelper()
{
  delete myXmlHandler;
  if ( myInterp && myPyModule ) {
    PyLockWrapper aLock; // Acquire GIL
    Py_XDECREF( myPyModule );
  }
}

/*!
  \brief Get the module being initialized.
  
  This is a little trick :) needed to provide an access from Python
  (SalomePyQt) to the module being currently activated. The problem
  that during the process of module initialization (initialize() 
  function) it is not yet available via application->activeModule()
  call.
  
  This method returns valid pointer only if called in scope of
  initialize() function or in several other specific cases.

  \return the module being currently initialized
*/
LightApp_Module* PyModuleHelper::getInitModule()
{
  QMutexLocker ml( &myInitMutex );
  return myInitModule;
}

/*!
  \brief Get default menu group identifier
  \return menu group ID (40 by default)
*/
int PyModuleHelper::defaultMenuGroup()
{
  return DEFAULT_GROUP; 
}

/*!
  \brief Get owner module
  \return owner module
*/
LightApp_Module* PyModuleHelper::module() const
{
  return myModule;
}

/*!
  \brief Get Python GUI module object
  \return python module
*/
PyObject* PyModuleHelper::pythonModule() const
{
  return myPyModule;
}

/*!
  \brief Connect action to the internal actionActivated() slot.

  Actions connected to internal actionActivated(), when activated, will
  be forwarded to the Python GUI module OnGUIEvent() function.

  \param a action being connected
*/
void PyModuleHelper::connectAction( QAction* a )
{
  if ( myModule && a )
    QObject::connect( a, SIGNAL( triggered( bool ) ), 
                      this, SLOT( actionActivated() ),
                      Qt::UniqueConnection );
}

/*!
  \brief Get the dockable windows associated with the module.
  
  To fill the list of windows the correspondind Python module's windows()
  method is called during the module initialization.

  By default, ObjectBrowser, PythonConsole and LogWindow windows are 
  associated to the module.

  Allowed dockable windows:
  - LightApp_Application::WT_ObjectBrowser : object browser
  - LightApp_Application::WT_PyConsole : python console
  - LightApp_Application::WT_LogWindow : log messages output window

  Dock area is defined by Qt::DockWidgetArea enumeration:
  - Qt::TopDockWidgetArea : top dock area
  - Qt::BottomDockWidgetArea : bottom dock area
  - Qt::LeftDockWidgetArea : left dock area
  - Qt::RightDockWidgetArea : right dock area

  \return map of dockable windows in form { <window_type> : <dock_area> }
*/
QMap<int, int> PyModuleHelper::windows() const
{
  FuncMsg fmsg( "PyModuleHelper::windows()" );

  return myWindowsMap;
}

/*!
  \brief Define the compatible view windows associated with the module.

  The associated view windows are opened automatically when the module
  is activated.

  To fill the list of views the correspondind Python module's views()
  method is called during the module initialization.
  By default, the list of view types is empty.

  \return list of view windows types
*/
QStringList PyModuleHelper::viewManagers() const
{
  FuncMsg fmsg( "PyModuleHelper::viewManagers()" );

  return myViewMgrList;
}

/*!
  \brief Initialization of the Python-based SALOME module.
  
  This method can be used for creation of the menus, toolbars and 
  other such stuff.
  
  There are two ways to do this:
  1) for obsolete modules, the implementation of this method first tries to read
  the <module>_<language>.xml resource file which contains a menu,
  toolbars and popup menus description;
  2) new modules can create menus by direct calling of the
  corresponding methods of SalomePyQt Python API in the Python
  module's initialize() method which is called from here.

  \note SALOME supports two modes of modules loading:
  - immediate (all the modules are created and initialized 
  immediately when the application object is created);
  - postponed modules loading (used currently); in this mode
  the module is loaded only by explicit request.
  If postponed modules loading is not used, the active
  study might be not yet defined at this stage, so initialize()
  method should not perform any study-based initialization.
  Such actions can be better done in activate() function.

  \param app parent application object
*/
void PyModuleHelper::initialize( CAM_Application* app )
{
  FuncMsg fmsg( "PyModuleHelper::initialize()" );

  // temporarily store module being currently activated
  // in the global variable to make it accessible from
  // Python API
  InitLocker lock( myModule );

  // try to get XML resource file name
  SUIT_ResourceMgr* resMgr = myModule->getApp()->resourceMgr();
  if ( !myXmlHandler && resMgr ) {
    // get current language
    QString lang = resMgr->stringValue( "language", "language", "en" );
    // get menu description file name
    QString aFileName = QString( "%1_%2.xml" ).arg( myModule->name() ).arg( lang );
    aFileName = resMgr->path( "resources", myModule->name(), aFileName );
    if ( !aFileName.isEmpty() && QFile::exists( aFileName ) ) {
      // create XML handler instance
      myXmlHandler = new XmlHandler( this, aFileName );
      // ask XML handler to create actions
      myXmlHandler->createActions();
    }
  }

  class InitializeReq : public PyInterp_Request
  {
  public:
    InitializeReq( PyModuleHelper*  _helper,
                   CAM_Application* _app )
      : PyInterp_Request( 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ),
        myApp( _app )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalInitialize( myApp );
    }
  private:
    PyModuleHelper*  myHelper;
    CAM_Application* myApp;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new InitializeReq( this, app ) );
}

/*!
  \brief Activation of the module.

  This function is usually used in order to show the module's 
  specific menus and toolbars, update actions state and perform
  other such actions required when the module is activated.
  
  \note Returning \c false from this function prevents the 
  module activation.

  \param study parent study
  \return \c true if activation is successful and \c false otherwise
*/
bool PyModuleHelper::activate( SUIT_Study* study )
{
  FuncMsg fmsg( "PyModuleHelper::activate()" );

  // reset the activation status to the default value
  myLastActivateStatus = true;

  class ActivateReq : public PyInterp_Request
  {
  public:
    ActivateReq( PyModuleHelper* _helper,
                 SUIT_Study*     _study,
                 bool            _customize )
  : PyInterp_Request( 0, true ), // this request should be processed synchronously (sync == true)
    myHelper( _helper ),
    myStudy ( _study ),
    myCustomize( _customize )
  {}
  protected:
    virtual void execute()
      {
        if ( !myCustomize )
          myHelper->internalActivate( myStudy );  // first activation stage
        else
          myHelper->internalCustomize( myStudy ); // second activation stage
      }
  private:
    PyModuleHelper* myHelper;
    SUIT_Study*     myStudy;
    bool            myCustomize;
  };

  // post request for activation (customize=false)
  PyInterp_Dispatcher::Get()->Exec( new ActivateReq( this, study, false ) );

  // check activation status (can be set to false by internalActivate())
  if ( myLastActivateStatus ) {
    // activate menus, toolbars, etc
    if ( myXmlHandler ) myXmlHandler->activateMenus( true );

    // show menus / toolbars
    myModule->setMenuShown( true );
    myModule->setToolShown( true );

    // post request for customization (customize=true)
    PyInterp_Dispatcher::Get()->Exec( new ActivateReq( this, study, true ) );

    // check activation status (can be set to false by internalCustomize())
    if ( myLastActivateStatus ) {
      // connect preferences changing signal
      connect( myModule->getApp(), SIGNAL( preferenceChanged( const QString&, const QString&, const QString& ) ),
               this,               SLOT(   preferenceChanged( const QString&, const QString&, const QString& ) ) );
      
      // connect active view change signal
      SUIT_Desktop* d = study->application()->desktop();
      connect( d,     SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
               this,  SLOT( activeViewChanged( SUIT_ViewWindow* ) ) );
      // if active window exists, call activeViewChanged() function;
      // temporary solution: if a getActiveView() in SalomePyQt available
      // we no longer need this 
      SUIT_ViewWindow* view = d->activeWindow();
      if ( view ) activeViewChanged( view );
      // get all view currently opened in the study and connect their signals to 
      // the corresponding slots of the class.
      foreach ( view, d->windows() ) connectView( view );
    }
    else {
      // hide menus / toolbars in case of error
      myModule->setMenuShown( false );
      myModule->setToolShown( false );
    }
  }

  return myLastActivateStatus;
}

/*!
  \brief Deactivation of the module.

  This function is usually used in order to hide the module's 
  specific menus and toolbars and perform other such actions
  required when the module is deactivated.

  \param study parent study
  \return \c true if deactivation is successful and \c false otherwise
*/
bool PyModuleHelper::deactivate( SUIT_Study* study )
{
  FuncMsg fmsg( "PyModuleHelper::deactivate()" );

  class DeactivateReq : public PyInterp_LockRequest
  {
  public:
    DeactivateReq( PyInterp_Interp* _py_interp,
                   PyModuleHelper*  _helper,
                   SUIT_Study*      _study )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ),
        myStudy ( _study )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalDeactivate( myStudy );
    }
  private:
    PyModuleHelper* myHelper;
    SUIT_Study*     myStudy;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new DeactivateReq( myInterp, this, study ) );

  // disconnect preferences changing signal
  disconnect( myModule->getApp(), SIGNAL( preferenceChanged( const QString&, const QString&, const QString& ) ),
              this,               SLOT(   preferenceChanged( const QString&, const QString&, const QString& ) ) );
  
  // disconnect the SUIT_Desktop signal windowActivated()
  SUIT_Desktop* d = study->application()->desktop();
  disconnect( d,     SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
              this,  SLOT( activeViewChanged( SUIT_ViewWindow* ) ) );

  // deactivate menus, toolbars, etc
  if ( myXmlHandler ) myXmlHandler->activateMenus( false );

  // hide menus / toolbars
  myModule->setMenuShown( false );
  myModule->setToolShown( false );

  return true;
}

/*!
  \brief Close of the module.

  This function is usually used in order to close the module's 
  specific menus and toolbars and perform other such actions
  required when the module is closed.
*/
void PyModuleHelper::modelClosed( SUIT_Study* study )
{
  FuncMsg fmsg( "PyModuleHelper::modelClosed()" );

  class StudyClosedReq : public PyInterp_LockRequest
  {
  public:
    StudyClosedReq( PyInterp_Interp* _py_interp,
                   PyModuleHelper*  _helper,
                   SUIT_Study*      _study )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ),
        myStudy ( _study )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalClosedStudy( myStudy );
    }
  private:
    PyModuleHelper* myHelper;
    SUIT_Study*     myStudy;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new StudyClosedReq( myInterp, this, study ) );

  // disconnect preferences changing signal
  disconnect( myModule->getApp(), SIGNAL( preferenceChanged( const QString&, const QString&, const QString& ) ),
              this,               SLOT(   preferenceChanged( const QString&, const QString&, const QString& ) ) );
  
  // disconnect the SUIT_Desktop signal windowActivated()
  SUIT_Desktop* d = study->application()->desktop();
  disconnect( d,     SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
	      this,  SLOT( activeViewChanged( SUIT_ViewWindow* ) ) );

  // deactivate menus, toolbars, etc
  if ( myXmlHandler ) myXmlHandler->activateMenus( false );

  // hide menus / toolbars
  myModule->setMenuShown( false );
  myModule->setToolShown( false );
}


/*!
  \brief Process module's preferences changing.

  Called when the module's own preferences are changed.
  
  \param section preference resources section
  \param parameter preference resources parameter name
*/
void PyModuleHelper::preferencesChanged( const QString& section, 
                                         const QString& parameter )
{
  FuncMsg fmsg( "PyModuleHelper::preferencesChanged()" );

  class PrefChangeReq : public PyInterp_LockRequest
  {
  public:
    PrefChangeReq( PyInterp_Interp* _py_interp,
                   PyModuleHelper*  _helper,
                   const QString&   _section,
                   const QString&   _parameter )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper ( _helper ),
        mySection( _section ),
        myParameter( _parameter ) 
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalPreferencesChanged( mySection, myParameter );
    }
  private:
    PyModuleHelper* myHelper;
    QString         mySection, myParameter;
  };

  // post the request only if dispatcher is not busy!
  // execute request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new PrefChangeReq( myInterp, this, section, parameter ) );
}

/*!
  \brief Process application preferences changing.

  Called when any application setting is changed.

  \param module preference module
  \param section preference resources section
  \param parameter preference resources parameter name
*/
void PyModuleHelper::preferenceChanged( const QString& module, 
                                        const QString& section,
                                        const QString& parameter )
{
  FuncMsg fmsg( "PyModuleHelper::preferenceChanged()" );

  // module's own preferences are processed by other preferencesChanged() method
  if ( module != myModule->moduleName() ) {
    // call helper
    preferencesChanged( section, parameter );
  }
}

/*!
  \brief Process study activation.
  
  Called when study desktop is activated. Used for notifying the Python
  module about changing of the active study.

  \param study study being activated
*/
void PyModuleHelper::studyActivated( SUIT_Study* study )
{
  FuncMsg fmsg( "PyModuleHelper::studyActivated()" );

  // StudyChangedReq: request class for internal studyChanged() operation
  class StudyChangedReq : public PyInterp_Request
  {
  public:
    StudyChangedReq( PyModuleHelper* _helper,
                     SUIT_Study*     _study )
      : PyInterp_Request(0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ), 
        myStudy ( _study )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalStudyChanged( myStudy );
    }
  private:
    PyModuleHelper* myHelper;
    SUIT_Study*     myStudy;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new StudyChangedReq( this, study ) );
}

/*!
  \brief Process action activation.
  
  Called when action is activated. Used for notifying the Python
  module about any related action activation.

  \sa connectAction()
*/
void PyModuleHelper::actionActivated()
{
  FuncMsg fmsg( "PyModuleHelper::actionActivated()" );

  // perform synchronous request to Python event dispatcher
  class ActionReq : public PyInterp_LockRequest
  {
  public:
    ActionReq( PyInterp_Interp* _py_interp,
               PyModuleHelper*  _helper,
               int              _id )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ),
        myId    ( _id  )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalActionActivated( myId );
    }
  private:
    PyModuleHelper* myHelper;
    int             myId;
  };

  // get sender action
  QAction* action = qobject_cast<QAction*>( sender() );
  if ( !action )
    return;

  // post request
  PyInterp_Dispatcher::Get()->Exec( new ActionReq( myInterp, this, myModule->actionId( action ) ) );
}

/*!
  \brief update selection from other views or modules.

  Called when selection is modified outside.
*/
void PyModuleHelper::selectionUpdated(const QStringList& entries)
{
  FuncMsg fmsg( "PyModuleHelper::selectionUpdated()" );
  MESSAGE("selectionUpdated");

  // perform synchronous request to Python event dispatcher
  class SelectionReq : public PyInterp_LockRequest
  {
  public:
    SelectionReq( PyInterp_Interp* _py_interp,
                  PyModuleHelper*  _helper,
                  const QStringList& _entries )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ),
        myEntries( _entries  )
    {
      MESSAGE("SelectionReq");
    }
  protected:
    virtual void execute()
    {
      MESSAGE("execute");
      myHelper->internalSelectionUpdated( myEntries );
    }
  private:
    PyModuleHelper* myHelper;
    const QStringList& myEntries;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new SelectionReq( myInterp, this, entries ) );
}

/*!
  \brief Process context popup menu request.
  
  Called when user activates popup menu in some window
  (view, object browser, etc).

  \param context popup menu context (e.g. "ObjectBrowser")
  \param menu popup menu
*/
void PyModuleHelper::contextMenu( const QString& context, QMenu* menu )
{
  FuncMsg fmsg( "PyModuleHelper::contextMenu()" );

  class ContextMenuReq : public PyInterp_LockRequest
  {
  public:
    ContextMenuReq( PyInterp_Interp* _py_interp,
                    PyModuleHelper*  _helper,
                    const QString&   _context,
                    QMenu*           _menu )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper ( _helper ),
        myContext( _context ),
        myMenu   ( _menu )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalContextMenu( myContext, myMenu );
    }
  private:
    PyModuleHelper* myHelper;
    QString         myContext;
    QMenu*          myMenu;
  };

  // post request only if dispatcher is not busy!
  // execute request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new ContextMenuReq( myInterp, this, context, menu ) );
}

/*!
  \brief Export preferences for the Python module.
  Called only once when the first instance of the module is created or
  when common Preferences dialog box is first time invoked.
*/
void PyModuleHelper::createPreferences()
{
  FuncMsg fmsg( "PyModuleHelper::createPreferences()" );

  // temporary set myInitModule because createPreferences() method
  // might be called during the module intialization process
  InitLocker lock( myModule );

  class CreatePrefReq : public PyInterp_LockRequest
  {
  public:
    CreatePrefReq( PyInterp_Interp* _py_interp,
                   PyModuleHelper*  _helper )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalCreatePreferences();
    }
  private:
    PyModuleHelper* myHelper;
  };

  // post request only if dispatcher is not busy!
  // execute request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new CreatePrefReq( myInterp, this ) );
}

/*!
  \brief Signal handler windowActivated(SUIT_ViewWindow*) of SUIT_Desktop

  Used to notify Python module that active view has been changed by the user.

  \param view view being activated
*/
void PyModuleHelper::activeViewChanged( SUIT_ViewWindow* view )
{
  FuncMsg fmsg( "PyModuleHelper::activeViewChanged()" );

  // perform synchronous request to Python event dispatcher
  class ActiveViewChangeReq : public PyInterp_LockRequest
  {
  public:
    ActiveViewChangeReq( PyInterp_Interp* _py_interp,
                         PyModuleHelper*  _helper,
                         SUIT_ViewWindow* _view )
      : PyInterp_LockRequest( _py_interp, 0, true ),
        myHelper( _helper ),
        myView( _view )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalActiveViewChanged( myView );
    }
  private:
    PyModuleHelper*  myHelper;
    SUIT_ViewWindow* myView;
  };

  // connect view (if it is not connected yet)
  connectView( view );
  
  PyInterp_Dispatcher::Get()->Exec( new ActiveViewChangeReq( myInterp, this, view ) ); 
}

/*!
  \brief Signal handler tryClose(SUIT_ViewWindow*) of a view
  \param view view being closed
*/
void PyModuleHelper::tryCloseView( SUIT_ViewWindow* view )
{
  FuncMsg fmsg( "PyModuleHelper::tryCloseView()" );

  class TryCloseViewReq : public PyInterp_LockRequest
  {
  public:
    TryCloseViewReq( PyInterp_Interp* _py_interp,
                     PyModuleHelper*  _helper,
                     SUIT_ViewWindow* _view )
      : PyInterp_LockRequest( _py_interp, 0, true ),
        myHelper( _helper ), 
        myView( _view )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalTryCloseView( myView );
    }
  private:
    PyModuleHelper*  myHelper;
    SUIT_ViewWindow* myView;    
  };

  PyInterp_Dispatcher::Get()->Exec( new TryCloseViewReq( myInterp, this, view ) );
}

/*!
  \brief Signal handler closing(SUIT_ViewWindow*) of a view
  \param view view being closed
*/
void PyModuleHelper::closeView( SUIT_ViewWindow* view )
{
  FuncMsg fmsg( "PyModuleHelper::closeView()" );

  class CloseViewReq : public PyInterp_LockRequest
  {
  public:
    CloseViewReq( PyInterp_Interp* _py_interp,
                  PyModuleHelper*  _helper,
                  SUIT_ViewWindow* _view )
      : PyInterp_LockRequest( _py_interp, 0, true ),
        myHelper( _helper ),
	myView( _view )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalCloseView( myView );
    }
  private:
    PyModuleHelper*  myHelper;
    SUIT_ViewWindow* myView;    
  };

  PyInterp_Dispatcher::Get()->Exec( new CloseViewReq( myInterp, this, view ) );
}

/*!
  \brief Signal handler cloneView() of OCCViewer_ViewWindow
  \param view view being cloned
*/
void PyModuleHelper::cloneView( SUIT_ViewWindow* view )
{
  FuncMsg fmsg( "PyModuleHelper::cloneView()" );

  class CloneViewReq : public PyInterp_LockRequest
  {
  public:
    CloneViewReq( PyInterp_Interp* _py_interp,
                  PyModuleHelper*  _helper,
                  SUIT_ViewWindow* _view )
      : PyInterp_LockRequest( _py_interp, 0, true ),
        myHelper( _helper ),
        myView( _view )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalCloneView( myView );
    }
  private:
    PyModuleHelper*  myHelper;
    SUIT_ViewWindow* myView;
  };
  
  PyInterp_Dispatcher::Get()->Exec( new CloneViewReq( myInterp, this, view ) );
}

/*!
  \brief Save module data. Called when user saves study.
  \param files output list of files where module stores data
  \param url study URL
*/
void PyModuleHelper::save( QStringList& files, const QString& url )
{
  FuncMsg fmsg( "PyModuleHelper::save()" );

  // temporary set myInitModule because save() method
  // might be called by the framework when this module is inactive,
  // but still it should be possible to access this module's data
  // from Python
  InitLocker lock( myModule );

  // perform synchronous request to Python event dispatcher
  class SaveReq: public PyInterp_LockRequest
  {
  public:     
    SaveReq( PyInterp_Interp* _py_interp,
             PyModuleHelper*  _helper,
             QStringList&     _files,
             const QString&   _url )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ) ,
        myFiles( _files ),
        myUrl( _url )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalSave( myFiles, myUrl );
    }
  private:
    PyModuleHelper* myHelper;
    QStringList&    myFiles;
    QString         myUrl;
  };
  
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new SaveReq( myInterp, this, files, url ) );
}

/*
  \brief Load module data. Called when user opens study 
  and activates module.
  \param files list of files where module data is stored
  \param url study URL
  \return \c true if loading has been finished successfully or \c false otherwise
*/
bool PyModuleHelper::load( const QStringList& files, const QString& url )
{
  FuncMsg fmsg( "PyModuleHelper::load()" );

  bool loaded = false;

  class LoadReq: public PyInterp_LockRequest
  {
  public:
    LoadReq( PyInterp_Interp* _py_interp,
             PyModuleHelper*  _helper,
             QStringList      _files,
             const QString&   _url,
             bool&            _loaded )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ) ,
        myFiles( _files ),
        myUrl( _url ),
        myLoaded( _loaded )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalLoad( myFiles, myUrl, myLoaded );
    }
  private:
    PyModuleHelper* myHelper;
    QStringList     myFiles;
    QString         myUrl;
    bool&           myLoaded;
  };
  
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new LoadReq( myInterp, this, files, url, loaded ) );

  return loaded;
}

/*!
  \brief Dump module data to the Python script. 
  Called when user activates dump study operation.
  \param files output list of files where module stores python script
*/
void PyModuleHelper::dumpPython( QStringList& files )
{
  FuncMsg fmsg( "PyModuleHelper::dumpPython()" );

  // temporary set myInitModule because dumpPython() method
  // might be called by the framework when this module is inactive,
  // but still it should be possible to access this module's data
  // from Python
  InitLocker lock( myModule );

  class DumpPythonReq: public PyInterp_LockRequest
  {
  public:     
    DumpPythonReq( PyInterp_Interp* _py_interp,
                   PyModuleHelper*  _helper,
                   QStringList&     _files )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ) ,
        myFiles( _files )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalDumpPython( myFiles );
    }
  private:
    PyModuleHelper* myHelper;
    QStringList&    myFiles;
  };
  
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new DumpPythonReq( myInterp, this, files ) );
}

/*!
  \brief Test if object \a what can be dragged by the user.
  \param what data object being tested
  \return \c true if object can be dragged or \c false otherwise
*/
bool PyModuleHelper::isDraggable( const SUIT_DataObject* what ) const
{
  FuncMsg fmsg( "PyModuleHelper::isDraggable()" );

  bool draggable = false;

  // perform synchronous request to Python event dispatcher
  class IsDraggableReq: public PyInterp_LockRequest
  {
  public:
    IsDraggableReq( PyInterp_Interp*     _py_interp,
                    PyModuleHelper*      _helper,
                    LightApp_DataObject* _data_object,
                    bool&                _is_draggable )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ) ,
        myDataObject( _data_object ),
        myIsDraggable( _is_draggable )
    {}
  protected:
    virtual void execute()
    {
      myIsDraggable = myHelper->internalIsDraggable( myDataObject );
    }
  private:
    PyModuleHelper*      myHelper;
    LightApp_DataObject* myDataObject;
    bool&                myIsDraggable;
  };

  const LightApp_DataObject* data_object = dynamic_cast<const LightApp_DataObject*>( what );
  if ( data_object ) {
    // Posting the request only if dispatcher is not busy!
    // Executing the request synchronously
    if ( !PyInterp_Dispatcher::Get()->IsBusy() )
      PyInterp_Dispatcher::Get()->Exec( new IsDraggableReq( myInterp,
                                        const_cast<PyModuleHelper*>( this ),
                                        const_cast<LightApp_DataObject*>( data_object ),
                                        draggable ) );
  }
  
  return draggable;
}

/*!
  \brief Test if drop operation can be done on the \a where object.
  \param where data object being tested
  \return \c true if if drop operation is supported by object or \c false otherwise
*/
bool PyModuleHelper::isDropAccepted( const SUIT_DataObject* where ) const
{
  FuncMsg fmsg( "PyModuleHelper::isDropAccepted()" );

  bool dropAccepted = false;

  // perform synchronous request to Python event dispatcher
  class IsDropAcceptedReq: public PyInterp_LockRequest
  {
  public:
    IsDropAcceptedReq( PyInterp_Interp*     _py_interp,
                       PyModuleHelper*      _helper,
                       LightApp_DataObject* _data_object,
                       bool&                _is_drop_accepted )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ) ,
        myDataObject( _data_object ),
        myIsDropAccepted( _is_drop_accepted )
    {}
  protected:
    virtual void execute()
    {
      myIsDropAccepted = myHelper->internalIsDropAccepted( myDataObject );
    }
  private:
    PyModuleHelper*      myHelper;
    LightApp_DataObject* myDataObject;
    bool&                myIsDropAccepted;
  };
  
  const LightApp_DataObject* data_object = dynamic_cast<const LightApp_DataObject*>( where );
  if ( data_object ) {
    // Posting the request only if dispatcher is not busy!
    // Executing the request synchronously
    if ( !PyInterp_Dispatcher::Get()->IsBusy() )
      PyInterp_Dispatcher::Get()->Exec( new IsDropAcceptedReq( myInterp,
                                         const_cast<PyModuleHelper*>( this ),
                                         const_cast<LightApp_DataObject*>( data_object ),
                                         dropAccepted ) );
  }

  return dropAccepted;
}

/*!
  \brief Perform drop operation
  \param what list of data objects being dropped
  \param where target data object for drop operation
  \param row line (child item index) where drop operation is performed to
  \param action current drop action (copy or move)
*/
void PyModuleHelper::dropObjects( const DataObjectList& what, SUIT_DataObject* where,
                                  const int row, Qt::DropAction action )
{
  FuncMsg fmsg( "PyModuleHelper::dropObjects()" );

  // perform synchronous request to Python event dispatcher
  class DropObjectsReq: public PyInterp_LockRequest
  {
  public:
    DropObjectsReq( PyInterp_Interp*      _py_interp,
                    PyModuleHelper*       _helper,
                    const DataObjectList& _what,
                    SUIT_DataObject*      _where,
                    const int             _row,
                    Qt::DropAction        _action )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ) ,
        myWhat( _what ),
        myWhere( _where ),
        myRow( _row ),
        myAction ( _action )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalDropObjects( myWhat, myWhere, myRow, myAction );
    }
  private:
    PyModuleHelper*  myHelper;
    DataObjectList   myWhat;
    SUIT_DataObject* myWhere;
    int              myRow;
    Qt::DropAction   myAction;
  };
  
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new DropObjectsReq( myInterp, this, what, where, row, action ) );
}

/*!
  \brief Get module engine IOR
  \return engine IOR as it is supplied by GUI Python module
 */
QString PyModuleHelper::engineIOR() const
{
  FuncMsg fmsg( "PyModuleHelper::engineIOR()" );

  class EngineIORReq : public PyInterp_LockRequest
  {
  public:
    EngineIORReq( PyInterp_Interp* _py_interp,
                  PyModuleHelper*  _helper,
                  QString&         _ior )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ),
        myIOR( _ior )
    {}
  protected:
    virtual void execute()
    {
      myIOR = myHelper->internalEngineIOR();
    }
  private:
    PyModuleHelper* myHelper;
    QString&        myIOR;
  };

  QString anIOR;
  // post request
  PyInterp_Dispatcher::Get()->Exec( new EngineIORReq( myInterp, 
						      const_cast<PyModuleHelper*>( this ),
						      anIOR ) );
  return anIOR;
}

/*!
  \brief Initialize python subinterpreter (one per study).
  \internal
*/
void PyModuleHelper::initInterp()
{
  FuncMsg fmsg( "--- PyModuleHelper::initInterp()" );

  QMutexLocker ml( &myInitMutex );

  myInterp = new SALOME_PYQT_PyInterp();
  myInterp->initialize();
  
#ifndef GUI_DISABLE_CORBA
  if ( !SUIT_PYTHON::initialized ) {
    // import 'salome' module and call 'salome_init' method;
    // do it only once on interpreter creation
    // ... first get python lock
    PyLockWrapper aLock; // Acquire GIL
    // ... then import a module
    PyObjWrapper aMod = PyImport_ImportModule( "salome" );
    if ( !aMod ) {
      // Error!
      PyErr_Print();
      return;
    }
    // ... then call a method
    int embedded = 1;
    PyObjWrapper aRes( PyObject_CallMethod( aMod, (char*)"salome_init", (char*)"si", NULL, embedded ) );
    if ( !aRes ) {
      // Error!
      PyErr_Print();
      return;
    }
  }
#endif 
}

/*!
  \brief Import Python GUI module and store reference to the module.
  \internal

  Warning! initInterp() should be called first!!!
*/
void PyModuleHelper::importModule()
{
  FuncMsg fmsg( "--- PyModuleHelper::importModule()" );

  // check if the subinterpreter is initialized
  if ( !myInterp ) {
    // Error! Python subinterpreter should be initialized first!
    myPyModule = 0;
    return;
  }

  // import Python GUI module and put it in <myPyModule> attribute
  // ... first get python lock
  PyLockWrapper aLock; // Acquire GIL
  // ... then import a module
  QString aMod = QString( "%1GUI" ).arg( myModule->name() );
  try {
    myPyModule = PyImport_ImportModule( aMod.toLatin1().data() );
  }
  catch (...) {
  }

  if ( !myPyModule ) {
    // Error!
    PyErr_Print();
    return;
  }
}

/*!
  \brief Set study workspace to the Python module.
  \internal

  Calls setWorkSpace() method of the Python module with 
  PyQt QWidget object to use with interpreter.

  Attention! initInterp() and importModule() should be called first!!!
*/
void PyModuleHelper::setWorkSpace()
{
  FuncMsg fmsg( "--- PyModuleHelper::setWorkSpace()" );

  if ( !IsCallOldMethods ) 
    return;

  // check if the subinterpreter is initialized and Python module is imported
  if ( !myInterp || !myPyModule ) {
    // Error! Python subinterpreter should be initialized and module should be imported first!
    return;
  }

  // call setWorkSpace() method
  // ... first get python lock
  PyLockWrapper aLock; // Acquire GIL

  // ... then try to import SalomePyQt module. If it's not possible don't go on.
  PyObjWrapper aQtModule( PyImport_ImportModule( "SalomePyQt" ) );
  if( !aQtModule ) {
    // Error!
    PyErr_Print();
    return;
  }

  // ... then get workspace object
  QWidget* aWorkspace = 0;
  if ( myModule->getApp()->desktop()->inherits( "STD_MDIDesktop" ) ) {
    STD_MDIDesktop* d = dynamic_cast<STD_MDIDesktop*>( myModule->getApp()->desktop() );
    if ( d )
      aWorkspace = d->workspace();
  }
  else if ( myModule->getApp()->desktop()->inherits( "STD_TabDesktop" ) ) {
    STD_TabDesktop* d = dynamic_cast<STD_TabDesktop*>( myModule->getApp()->desktop() );
    if ( d )
      aWorkspace = d->workstack();
  }
#if SIP_VERSION >= 0x041300
  static const sipTypeDef *sipType_QWidget = 0;
  if (!sipType_QWidget)
    sipType_QWidget = sipFindType("QWidget");
#endif
  PyObjWrapper pyws( sipBuildResult( 0, "D", aWorkspace, sipType_QWidget , NULL) );
  // ... and finally call Python module's setWorkSpace() method (obsolete)
  if ( PyObject_HasAttrString( myPyModule, (char*)"setWorkSpace" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"setWorkSpace", (char*)"O", pyws.get() ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Initialization callback function
  \internal

  Performs the following actions:
  - initialize or get the Python interpreter (one per study)
  - import the Python module
  - pass the workspace widget to the Python module
  - call Python module's initialize() method
  - call Python module's windows() method
  - call Python module's views() method

  \param app parent application object
*/
void PyModuleHelper::internalInitialize( CAM_Application* app )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalInitialize()" );

  // reset interpreter to NULL
  myInterp = 0;

  // get study Id
  LightApp_Application* anApp = dynamic_cast<LightApp_Application*>( app );
  if ( !anApp )
    return;
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( app->activeStudy() );
  if ( !aStudy )
    return;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp();
  if ( !myInterp )
    return; // Error

  // import Python GUI module
  importModule();
  if ( !myPyModule )
    return; // Error

  // then call Python module's initialize() method
  // ... first get python lock
  PyLockWrapper aLock; // Acquire GIL

  // ... (the Python module is already imported)
  // ... finally call Python module's initialize() method
  if ( PyObject_HasAttrString( myPyModule, (char*)"initialize" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"initialize", (char*)"" ) );
    if ( !res ) {
      PyErr_Print();
    }
  }

  // get required dockable windows list from the Python module 
  // by calling windows() method
  // ... first put default values
  myWindowsMap.insert( LightApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  myWindowsMap.insert( LightApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
  myWindowsMap.insert( LightApp_Application::WT_LogWindow,     Qt::BottomDockWidgetArea );

  if ( PyObject_HasAttrString( myPyModule , (char*)"windows" ) ) {
    PyObjWrapper res1( PyObject_CallMethod( myPyModule, (char*)"windows", (char*)"" ) );
    if ( !res1 ) {
      PyErr_Print();
    }
    else {
      myWindowsMap.clear();
      if ( PyDict_Check( res1 ) ) {
        PyObject* key;
        PyObject* value;
        Py_ssize_t pos = 0;
        while ( PyDict_Next( res1, &pos, &key, &value ) ) {
          // parse the return value
          // it should be a map: {integer:integer}
          int aKey, aValue;
          if( key && PyLong_Check( key ) && value && PyLong_Check( value ) ) {
            aKey   = PyLong_AsLong( key );
            aValue = PyLong_AsLong( value );
            myWindowsMap[ aKey ] = aValue;
          }
        }
      }
    }
  }

  // get compatible view windows types from the Python module 
  // by calling views() method
  if ( PyObject_HasAttrString( myPyModule , (char*)"views" ) ) {
    PyObjWrapper res2( PyObject_CallMethod( myPyModule, (char*)"views", (char*)"" ) );
    if ( !res2 ) {
      PyErr_Print();
    }
    else {
      // parse the return value
      // result can be one string...
      if ( PyUnicode_Check( res2 ) ) {
        myViewMgrList.append( PyUnicode_AsUTF8( res2 ) );
      }
      // ... or list of strings
      else if ( PyList_Check( res2 ) ) {
        int size = PyList_Size( res2 );
        for ( int i = 0; i < size; i++ ) {
          PyObject* value = PyList_GetItem( res2, i );
          if( value && PyUnicode_Check( value ) ) {
            myViewMgrList.append( PyUnicode_AsUTF8( value ) );
          }
        }
      }
    }
  }
}

/*!
  \brief Activation callback function
  \internal

  Performs the following actions:
  - initialize or get the Python interpreter (one per study)
  - import the Python GUI module
  - call Python module's activate() method

  \param study parent study
*/
void PyModuleHelper::internalActivate( SUIT_Study* study )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalActivate()" );

  // get study Id
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( study );
  if ( !aStudy )
    return;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp();
  if ( !myInterp ) {
    myLastActivateStatus = false;
    return; // Error
  }

  // import Python GUI module
  importModule();
  if ( !myPyModule ) {
    myLastActivateStatus = false;
    return; // Error
  }

  // get python lock
  PyLockWrapper aLock; // Acquire GIL

  // call Python module's activate() method (for the new modules)
  if ( PyObject_HasAttrString( myPyModule , (char*)"activate" ) ) {
    PyObject* res1 = PyObject_CallMethod( myPyModule, (char*)"activate", (char*)"" );
    if ( !res1 || !PyBool_Check( res1 ) ) {
      PyErr_Print();
      // always true for old modules (no return value)
      myLastActivateStatus = true;
    }
    else {
      // detect return status
      myLastActivateStatus = PyObject_IsTrue( res1 );
    }
  } 
}

/*!
  \brief Additional menu customization callback function
  \internal

  Performs the following actions:
  - get the Python interpreter (one per study)
  - import the Python GUI module
  - call Python module's setSettings() method (obsolete function, 
  used for compatibility with old code)

  \param study parent study
*/
void PyModuleHelper::internalCustomize( SUIT_Study* study )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalCustomize()" );

  // get study Id
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( study );
  if ( !aStudy )
    return;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp();
  if ( !myInterp ) {
    myLastActivateStatus = false;
    return; // Error
  }

  // import Python GUI module
  importModule();
  if ( !myPyModule ) {
    myLastActivateStatus = false;
    return; // Error
  }

  // call Python module's setWorkSpace() method (obsolete)
  setWorkSpace();

  // get python lock
  PyLockWrapper aLock; // Acquire GIL

  if ( IsCallOldMethods ) {
    // call Python module's setSettings() method (obsolete)
    if ( PyObject_HasAttrString( myPyModule , (char*)"setSettings" ) ) {
      PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"setSettings", (char*)"" ) );
      if( !res ) {
        PyErr_Print();
      }
      myLastActivateStatus = myLastActivateStatus && true;
    }
  }
}

/*!
  \brief Deactivation callback function
  \internal

  Performs the following actions:
  - call Python module's deactivate() method

  \param study parent study
*/
void PyModuleHelper::internalDeactivate( SUIT_Study* /*study*/ )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalDeactivate()" );

  // check that Python subinterpreter is initialized and Python module is imported
  if ( !myInterp || !myPyModule ) {
    // Error! Python subinterpreter should be initialized and module should be imported first!
    return;
  }
  // then call Python module's deactivate() method
  if ( PyObject_HasAttrString( myPyModule , (char*)"deactivate" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"deactivate", (char*)"" ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Internal closure:

  Performs the following actions:
  - call Python module's closeStudy() method

  \param theStudy parent study object
*/
void PyModuleHelper::internalClosedStudy( SUIT_Study* theStudy )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalClosedStudy()" );

  // Get study Id
  // get study Id
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( theStudy );
  if ( !aStudy )
    return;

  // check that Python subinterpreter is initialized and Python module is imported
  if ( !myInterp || !myPyModule ) {
    // Error! Python subinterpreter should be initialized and module should be imported first!
    return;
  }
  // then call Python module's deactivate() method
  if ( PyObject_HasAttrString( myPyModule , (char*)"closeStudy" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"closeStudy", (char*)"" ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}



/*!
  \brief Preference changing callback function.
  \internal

  Performs the following actions:
  - call Python module's preferenceChanged() method

  \param section resources section name
  \param setting resources parameter name
*/
void PyModuleHelper::internalPreferencesChanged( const QString& section, const QString& setting )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalPreferencesChanged()" );

  // check that Python subinterpreter is initialized and Python module is imported
  if ( !myInterp || !myPyModule ) {
    // Error! Python subinterpreter should be initialized and module should be imported first!
    return;
  }

  if ( PyObject_HasAttrString( myPyModule, (char*)"preferenceChanged" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule,
                                           (char*)"preferenceChanged", 
                                           (char*)"ss", 
                                           section.toUtf8().constData(), 
                                           setting.toUtf8().constData() ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Active study change callback function.
  \internal

  Called when active the study is actived (user brings its 
  desktop to top):
  - initialize or get the Python interpreter (one per study)
  - import the Python GUI module
  - call Python module's activeStudyChanged() method

  \param study study being activated
*/
void PyModuleHelper::internalStudyChanged( SUIT_Study* study )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalStudyChanged()" );

  // get study Id
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( study );
  if ( !aStudy )
    return;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp();
  if ( !myInterp )
    return; // Error

  // import Python GUI module
  importModule();
  if ( !myPyModule )
    return; // Error

  // call Python module's setWorkSpace() method
  setWorkSpace();

  // get python lock
  PyLockWrapper aLock; // Acquire GIL

  // call Python module's activeStudyChanged() method
  if ( PyObject_HasAttrString( myPyModule, (char*)"activeStudyChanged" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"activeStudyChanged", (char*)"" ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief GUI event handling callback function
  \internal 

  Performs the following actions:
  - calls Python module's OnGUIEvent() method

  \param id GUI action ID
*/
void PyModuleHelper::internalActionActivated( int id )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalActionActivated()" );
  fmsg.message( QString( "action id = %1" ).arg( id ) );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myPyModule )
    return; // Error

  if ( PyObject_HasAttrString( myPyModule, (char*)"OnGUIEvent" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"OnGUIEvent", (char*)"i", id ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief update selection from other views or modules
  \internal

  Performs the following actions:
  - calls Python module's onSelectionpdated(entries) method

  \param list of entries
*/
void PyModuleHelper::internalSelectionUpdated(const QStringList& entries)
{
  FuncMsg fmsg("--- PyModuleHelper::internalSelectionUpdated()");
  MESSAGE("internalSelectionUpdated");

  // Python interpreter should be initialized and Python module should be imported first
  if (!myInterp || !myPyModule)
    return; // Error

  QStringList* theList = new QStringList(entries);

#if SIP_VERSION >= 0x041300
  static const sipTypeDef *sipType_QStringList = 0;
  if (!sipType_QStringList)
    sipType_QStringList = sipFindType("QStringList");
#endif
  PyObjWrapper sipList( sipBuildResult( 0, "D", theList, sipType_QStringList, NULL ) );
  if (PyObject_HasAttrString(myPyModule, (char*) "onSelectionUpdated"))
    {
      MESSAGE("call onSelectionUpdated");
      PyObjWrapper res(PyObject_CallMethod(myPyModule, (char*) "onSelectionUpdated", (char*) "O", sipList.get()));

      if (!res)
        {
          PyErr_Print();
        }
    }
}

/*!
  \brief Context popup menu handling callback function
  \internal

  Performs the following actions:
  - calls Python module's definePopup(...) method (obsolete function, 
  used for compatibility with old code) to define the popup menu context
  - parses XML resourses file (if exists) and fills the popup menu with the items)
  - calls Python module's customPopup(...) method (obsolete function, 
  used for compatibility with old code) to allow module to customize the popup menu
  - for new modules calls createPopupMenu() function to allow the 
  modules to build the popup menu by using insertItem(...) Qt functions.

  \param context popup menu context
  \param menu popup menu
*/
void PyModuleHelper::internalContextMenu( const QString& context, QMenu* menu )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalContextMenu()" );
  fmsg.message( QString( "context: %1" ).arg( context ) );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myPyModule )
    return; // Error

  QString aContext( "" ), aObject( "" ), aParent( context );

  if ( IsCallOldMethods && PyObject_HasAttrString( myPyModule, (char*)"definePopup" ) ) {
    // call definePopup() Python module's function
    // this is obsolete function, used only for compatibility reasons
    PyObjWrapper res( PyObject_CallMethod( myPyModule,
                                           (char*)"definePopup",
                                           (char*)"sss",
                                           context.toUtf8().constData(),
                                           aObject.toUtf8().constData(),
                                           aParent.toUtf8().constData() ) );
    if( !res ) {
      PyErr_Print();
    }
    else {
      // parse return value
      char *co, *ob, *pa;
      if( PyArg_ParseTuple( res, "sss", &co, &ob, &pa ) ) {
        aContext = co;
        aObject  = ob;
        aParent  = pa;
      }
    }
  } // if ( IsCallOldMethods ... )

  // first try to create menu via XML parser:
  // we create popup menus without help of QtxPopupMgr
  if ( myXmlHandler )
    myXmlHandler->createPopup( menu, aContext, aParent, aObject );

#if SIP_VERSION >= 0x041300
  static const sipTypeDef *sipType_QMenu = 0;
  if (!sipType_QMenu)
    sipType_QMenu = sipFindType("QMenu");
#endif
  PyObjWrapper sipPopup( sipBuildResult( 0, "D", menu, sipType_QMenu, NULL ) );

  // then call Python module's createPopupMenu() method (for new modules)
  if ( PyObject_HasAttrString( myPyModule, (char*)"createPopupMenu" ) ) {
    PyObjWrapper res1( PyObject_CallMethod( myPyModule,
                                            (char*)"createPopupMenu",
                                            (char*)"Os",
                                            sipPopup.get(),
                                            context.toUtf8().constData() ) );
    if( !res1 ) {
      PyErr_Print();
    }
  }

  if ( IsCallOldMethods && PyObject_HasAttrString( myPyModule, (char*)"customPopup" ) ) {
    // call customPopup() Python module's function
    // this is obsolete function, used only for compatibility reasons
    PyObjWrapper res2( PyObject_CallMethod( myPyModule,
                                            (char*)"customPopup",
                                            (char*)"Osss",
                                            sipPopup.get(),
                                            aContext.toUtf8().constData(),
                                            aObject.toUtf8().constData(),
                                            aParent.toUtf8().constData() ) );
    if( !res2 ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Preferences initialization callback function.
  \internal

  Performs the following actions:
  - calls Python module's createPreferences() method
*/
void PyModuleHelper::internalCreatePreferences()
{
  FuncMsg fmsg( "--- PyModuleHelper::internalCreatePreferences()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myPyModule )
    return; // Error

  if ( PyObject_HasAttrString( myPyModule, (char*)"createPreferences" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"createPreferences", (char*)"" ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Active view changing callback function
  \internal
  \param view view being activated
*/
void PyModuleHelper::internalActiveViewChanged( SUIT_ViewWindow* view )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalActiveViewChanged()" );

  if ( !myInterp || !myPyModule || !view ) 
    return;
  
  fmsg.message( QString( "view id: %1" ).arg( view->getId() ) );

  if ( PyObject_HasAttrString( myPyModule, (char*)"activeViewChanged" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"activeViewChanged", (char*)"i" , view->getId() ) );
    if ( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief View closing callback function
  \internal
  \param view view user tries to close
*/
void PyModuleHelper::internalTryCloseView( SUIT_ViewWindow* view )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalTryCloseView()" );

  if ( !myInterp || !myPyModule || !view ) 
    return;  

  fmsg.message( QString( "view id: %1" ).arg( view->getId() ) );

  if ( PyObject_HasAttrString( myPyModule, (char*)"viewTryClose" ) ) 
  {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"viewTryClose", (char*)"i", view->getId() ) );
    if ( !res )
    {
      PyErr_Print();
    }
  }
}

/*!
  \brief View closing callback function
  \internal
  \param view view being closed
*/
void PyModuleHelper::internalCloseView( SUIT_ViewWindow* view )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalCloseView()" );

  if ( !myInterp || !myPyModule || !view ) 
    return;  

  fmsg.message( QString( "view id: %1" ).arg( view->getId() ) );

  if ( PyObject_HasAttrString( myPyModule, (char*)"viewClosed" ) ) 
  {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"viewClosed", (char*)"i", view->getId() ) );
    if ( !res )
    {
      PyErr_Print();
    }
  }
}

/*!
  \brief View cloning callback function
  \internal
  \param view view being cloned
*/
void PyModuleHelper::internalCloneView( SUIT_ViewWindow* view )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalCloneView()" );

  if ( !myInterp || !myPyModule || !view ) 
    return;  

  fmsg.message( QString( "view id: %1" ).arg( view->getId() ) );

  if ( PyObject_HasAttrString( myPyModule, (char*)"viewCloned" ) ) 
  {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"viewCloned", (char*)"i", view->getId() ) );
    if( !res )
      PyErr_Print();
  }
}

/*!
  \brief Module data saving callback function.
  \internal
  \param files output list of files where module stores data
  \param url study URL
*/
void PyModuleHelper::internalSave( QStringList& files, const QString& url )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalSave()" );

  // Python interpreter should be initialized and Python module should be
  // import firs
  // files list should contain a path to the temporary directory as a first entry
  if ( !myInterp || !myPyModule || files.isEmpty() )
    return;

  if ( PyObject_HasAttrString(myPyModule, (char*)"saveFiles") ) {

    // try with two parameters (new syntax)
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"saveFiles",
                                           (char*)"ss",
                                           files.first().toUtf8().constData(),
                                           url.toUtf8().constData() ) );
    if ( !res )
      // try with single parameter (old syntax)
      res = PyObject_CallMethod( myPyModule, (char*)"saveFiles",
                                 (char*)"s", files.first().toUtf8().constData() );
    
    if ( !res ) {
      PyErr_Print();
    }
    else {
      // parse the return value
      // result can be one string...
      if ( PyUnicode_Check( res ) ) {
        QString astr = PyUnicode_AsUTF8( res );
        files.append( astr );
      }
      //also result can be a list...
      else if ( PyList_Check( res ) ) {
        int size = PyList_Size( res );
        for ( int i = 0; i < size; i++ ) {
          PyObject* value = PyList_GetItem( res, i );
          if ( value && PyUnicode_Check( value ) ) {
            files.append( PyUnicode_AsUTF8( value ) );
          }
        }
      }
    }
  }
}

/*!
  \brief Module data loading callback function.
  \internal
  \param files list of files where module data is stored
  \param url study URL
  \param opened output success flag
*/
void PyModuleHelper::internalLoad( const QStringList& files, const QString& url, bool& opened )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalLoad()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myPyModule || files.isEmpty() )
    return;

  QStringList* theList = new QStringList( files );

#if SIP_VERSION >= 0x041300
  static const sipTypeDef *sipType_QStringList = 0;
  if (!sipType_QStringList)
    sipType_QStringList = sipFindType("QStringList");
#endif
  PyObjWrapper sipList( sipBuildResult( 0, "D", theList, sipType_QStringList, NULL ) );
  if ( PyObject_HasAttrString(myPyModule , (char*)"openFiles") ) {

    // try with two parameters (new syntax)
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"openFiles",
                                           (char*)"Os", sipList.get(),
                                           url.toUtf8().constData() ) );

    if ( !res )
      // try with single parameter (old syntax)
      res = PyObject_CallMethod( myPyModule, (char*)"openFiles",
                                 (char*)"O", sipList.get() );

    if ( !res || !PyBool_Check( res ) ) {
      PyErr_Print();
      opened = false;
    }
    else {
      opened = PyObject_IsTrue( res );
    }
  }
}

/*!
  \brief Module dump python callback function.
  \internal
  \param files output list of files where module stores python script
*/
void PyModuleHelper::internalDumpPython( QStringList& files )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalDumpPython()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  // files list should contain a path to the temporary directory
  if ( !myInterp || !myPyModule || files.isEmpty() )
    return;

  if ( PyObject_HasAttrString(myPyModule, (char*)"dumpStudy") ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"dumpStudy",
                                           (char*)"s", files.first().toUtf8().constData()));

    if ( !res ) {
      PyErr_Print();
    }
    else {
      // parse the return value
      // result can be one string...
      if ( PyUnicode_Check( res ) ) {
        QString astr = PyUnicode_AsUTF8( res );
        //SCRUTE(astr);
        files.append(astr);
      }
      //also result can be a list...
      else if ( PyList_Check( res ) ) {
        int size = PyList_Size( res );
        for ( int i = 0; i < size; i++ ) {
          PyObject* value = PyList_GetItem( res, i );
          if( value && PyUnicode_Check( value ) ) {
            files.append( PyUnicode_AsUTF8( value ) );
          }
        }
      }
    }
  }
}

/*!
  \brief Check data object's 'draggable' status callback function.
  \internal
  \param what data object being tested
  \return \c true if object can be dragged or \c false otherwise
*/
bool PyModuleHelper::internalIsDraggable( LightApp_DataObject* what )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalIsDraggable()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myPyModule || !what )
    return false;

  bool draggable = false;

  if ( PyObject_HasAttrString(myPyModule , (char*)"isDraggable") ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"isDraggable",
                      (char*)"s", what->entry().toUtf8().constData() ) );
    if( !res || !PyBool_Check( res )) {
      PyErr_Print();
      draggable = false;
    }
    else{
      draggable = PyObject_IsTrue( res );
    }
  }

  return draggable;
}

/*!
  \brief Check data object's 'drop allowed' status callback function.
  \internal
  \param where data object being tested
  \return \c true if if drop operation is supported by object or \c false otherwise
*/
bool PyModuleHelper::internalIsDropAccepted( LightApp_DataObject* where )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalIsDropAccepted()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myPyModule || !where )
    return false;

  bool dropAccepted = false;

  if ( PyObject_HasAttrString(myPyModule , (char*)"isDropAccepted") ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"isDropAccepted",
                      (char*)"s", where->entry().toUtf8().constData() ) );
    if( !res || !PyBool_Check( res )) {
      PyErr_Print();
      dropAccepted = false;
    }
    else{
      dropAccepted = PyObject_IsTrue( res );
    }
  }

  return dropAccepted;
}

/*!
  \brief Data dropping callback function.
  \internal
  \param what list of data objects being dropped
  \param where target data object for drop operation
  \param row line (child item index) where drop operation is performed to
  \param action current drop action (copy or move)
*/
void PyModuleHelper::internalDropObjects( const DataObjectList& what, SUIT_DataObject* where,
                                          const int row, Qt::DropAction action )
{
  FuncMsg fmsg( "--- PyModuleHelper::internalDropObjects()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myPyModule || what.isEmpty() || !where )
    return;

  QStringList* theList = new QStringList();

  LightApp_DataObject* whereObject = dynamic_cast<LightApp_DataObject*>( where );
  if ( !whereObject ) return;
  
  for ( int i = 0; i < what.count(); i++ ) {
    LightApp_DataObject* dataObject = dynamic_cast<LightApp_DataObject*>( what[i] );
    if ( dataObject ) theList->append( dataObject->entry() );
  }

#if SIP_VERSION >= 0x041300
  static const sipTypeDef *sipType_QStringList = 0;
  if (!sipType_QStringList)
    sipType_QStringList = sipFindType("QStringList");
#endif
  PyObjWrapper sipList( sipBuildResult( 0, "D", theList, sipType_QStringList, NULL) );
  if ( PyObject_HasAttrString(myPyModule, (char*)"dropObjects") ) {
      PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"dropObjects", (char*)"Osii",
                        sipList.get(),
                        whereObject->entry().toUtf8().constData(),
                        row, action ) );
    
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Get engine IOR callback function
  \internal
  
  Tries to get engine IOR from the Python module using engineIOR() function.
  That function can load module engine using appropriate container if required.

  \return engine IOR or empty string if it is not provided by Python module 
*/
QString PyModuleHelper::internalEngineIOR() const
{
  FuncMsg fmsg( "--- PyModuleHelper::internalEngineIOR()" );

  QString ior;

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( myInterp && myModule ) {
    if ( PyObject_HasAttrString( myPyModule , "engineIOR" ) ) {
      PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"engineIOR", (char*)"" ) );
      if ( !res ) {
          PyErr_Print();
      }
      else {
        // parse the return value, result chould be string
        if ( PyUnicode_Check( res ) ) {
          ior = PyUnicode_AsUTF8( res );
        }
      }
    }
  }
  return ior;
}

/*!
  \brief Connects signals about activating and cloning view on internal slots
  \param view view being connected
*/
void PyModuleHelper::connectView( SUIT_ViewWindow* view )
{
  SUIT_ViewManager* viewMgr = view->getViewManager();
  SUIT_ViewModel* viewModel = viewMgr ? viewMgr->getViewModel() : 0;
      
  // Connect tryCloseView() and deleteView() signals
  if ( viewMgr ) {
    connect( viewMgr, SIGNAL( tryCloseView( SUIT_ViewWindow* ) ),
             this, SLOT( tryCloseView( SUIT_ViewWindow* ) ),
             Qt::UniqueConnection );
    connect( viewMgr, SIGNAL( deleteView( SUIT_ViewWindow* ) ),
             this, SLOT( closeView( SUIT_ViewWindow* ) ),
             Qt::UniqueConnection );
  }
  
  // Connect cloneView() signal of an OCC View
  if ( view->inherits( "OCCViewer_ViewWindow" ) ) {
    connect( view, SIGNAL( viewCloned( SUIT_ViewWindow* ) ), 
             this, SLOT( cloneView( SUIT_ViewWindow* ) ),
             Qt::UniqueConnection );
  }
  // Connect cloneView() signal of Plot2d View 
  else if ( viewModel && viewModel->inherits( "Plot2d_Viewer" ) ) {
    connect( viewModel, SIGNAL( viewCloned( SUIT_ViewWindow* ) ), 
             this, SLOT( cloneView( SUIT_ViewWindow* ) ),
             Qt::UniqueConnection );
  }
}



void PyModuleHelper::internalOBClickedPython( const QString& theObj, int theColumn)
{
  FuncMsg fmsg( "--- PyModuleHelper::internalOBClickedPython()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myPyModule )
    return; // Error

  if ( PyObject_HasAttrString( myPyModule, (char*)"onObjectBrowserClicked" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myPyModule, (char*)"onObjectBrowserClicked", (char*)"si", theObj.toUtf8().constData(), theColumn ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}



void PyModuleHelper::onObjectBrowserClicked(SUIT_DataObject* theObj, int theColumn)
{
  FuncMsg fmsg( "PyModuleHelper::onObjectBrowserClicked()" );

  // temporary set myInitModule because dumpPython() method
  // might be called by the framework when this module is inactive,
  // but still it should be possible to access this module's data
  // from Python
  InitLocker lock( myModule );

  class PythonReq: public PyInterp_LockRequest
  {
  public:     
    PythonReq( PyInterp_Interp* _py_interp,
               PyModuleHelper*  _helper,
               const QString& _entry,
               int     _column )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myHelper( _helper ) ,
        myEntry( _entry ),
        myColumn( _column )
    {}
  protected:
    virtual void execute()
    {
      myHelper->internalOBClickedPython( myEntry, myColumn );
    }
  private:
    PyModuleHelper* myHelper;
    QString myEntry;
    int myColumn;
  };
  
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  const LightApp_DataObject* data_object = dynamic_cast<const LightApp_DataObject*>( theObj );
  if ( (!PyInterp_Dispatcher::Get()->IsBusy()) && data_object )
    PyInterp_Dispatcher::Get()->Exec( new PythonReq( myInterp, this, data_object->entry(), theColumn ) );
}

