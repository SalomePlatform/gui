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
// File:      QtxActionMgr.cxx
// Author:    Alexander SOLOVYOV, Sergey TELKOV

#include "Qtx.h"
#include "QtxActionMgr.h"
#include "QtxAction.h"
#include <QFile>
#include <QTimer>
#ifndef QT_NO_DOM
#include <QDomDocument>
#include <QDomNode>
#include <QCoreApplication>
#endif

typedef QList< QPointer<QAction> > qtx_actionlist;
static qtx_actionlist qtx_separator_actions;

/*!
  \internal
  \brief Clean all cashed separator actions.
 */
void qtxSeparatorActionCleanup()
{
  for ( qtx_actionlist::iterator it = qtx_separator_actions.begin(); it != qtx_separator_actions.end(); ++it )
    delete *it;
}

/*!
  \class QtxActionMgr::SeparatorAction
  \internal
  \brief Separator action class.
*/

class QtxActionMgr::SeparatorAction : public QtxAction
{
public:
  SeparatorAction( QObject* = 0 );
  virtual ~SeparatorAction();
};

/*!
  \brief Separator action constructor.
  \param parent parent object
*/
QtxActionMgr::SeparatorAction::SeparatorAction( QObject* parent )
: QtxAction( parent )
{
  setSeparator( true );
}

/*!
  \brief Separator action destructor.

  Does nothing for the moment.
*/
QtxActionMgr::SeparatorAction::~SeparatorAction()
{
}


/*!
  \class QtxActionMgr
  \brief Manages a set of actions accessible by unique ID.
  
  Base class for menu, toolbar action containers and popup menu creators.
*/

/*!
  \brief Action manager constructor.
  \param parent parent object
*/
QtxActionMgr::QtxActionMgr( QObject* parent )
: QObject( parent ),
  myUpdate( true ),
  myUpdTimer( 0 )
{
}

/*!
  \brief Action manager destructor.

  Does nothing for the moment.
*/
QtxActionMgr::~QtxActionMgr()
{
}

/*!
  \brief Register action in the internal map.

  If \a userId is less than 0, the ID for the action is generated automatically.
  If action with given \a userId is already registered, it will be re-registered.

  \param a action to be registered
  \param userId action ID
  \return action ID (the same as userId or generated one)
*/
int QtxActionMgr::registerAction( QAction* a, const int userId )
{
  if ( !a )
    return -1;

  int theId = userId < 0 ? generateId() : userId;

  if ( contains( theId ) )
    unRegisterAction( theId );

  int cur = actionId( a );
  if ( cur != -1 )
  {
    if ( userId == -1 )
      return cur;
    else
      unRegisterAction( cur );
  }

  myActions.insert( theId, a );

  return theId;
}

/*!
  \brief Unregister action from internal map.
  \param id action ID
*/
void QtxActionMgr::unRegisterAction( const int id )
{
  if( contains( id ) )
    myActions.remove( id );
}

/*!
  \brief Get action by \a id.
  \param id action ID
  \return action (0 if action is not found)
*/
QAction* QtxActionMgr::action( const int id ) const
{
  if ( contains( id ) )
    return myActions[ id ];
  else
    return 0;
}

/*!
  \brief Get action ID.
  \param a action
  \return action ID (-1 if action is not found)
*/
int QtxActionMgr::actionId( const QAction* a ) const
{
  if ( !a )
    return -1;

  int theId = -1;
  for ( ActionMap::ConstIterator it = myActions.begin(); it != myActions.end() && theId == -1; ++it )
  {
    if ( it.value() == a )
      theId = it.key();
  }

  return theId;
}

/*!
  \brief Check if an action with given \a id is registered in the action manager.
  \param id action ID
  \return \c true if internal map contains action with such ID
*/
bool QtxActionMgr::contains( const int id ) const
{
  return myActions.contains( id );
}

/*!
  \brief Get number of registered actions.
  \return number of actions in the internal map
*/
int QtxActionMgr::count() const
{
  return myActions.count();
}

/*!
  \brief Check if there are no actions registered in the action manager.
  \return \c true if internal map is empty
*/
bool QtxActionMgr::isEmpty() const
{
  return myActions.isEmpty();
}

/*!
  \brief Get all registered actions IDs.
  \param list of actions IDs to be fiiled in
*/
void QtxActionMgr::idList( QIntList& lst ) const
{
  lst = myActions.keys();
}

/*!
  \brief Check if update is enabled.
  \return \c true if update is enabled
*/
bool QtxActionMgr::isUpdatesEnabled() const
{
  return myUpdate;
}

/*!
  \brief Enable/disable update operation.
  \param upd new state
*/
void QtxActionMgr::setUpdatesEnabled( const bool upd )
{
  myUpdate = upd;
}

/*!
  \brief Check if an action with given ID \a actId is visible to
         the parent action with given ID \a place.

  Base implementatin always returns \c true.

  \param actId action ID
  \param place some parent action ID
  \return \c true if an action is visible
*/
bool QtxActionMgr::isVisible( const int /*actId*/, const int /*place*/ ) const
{
  return true;
}

/*!
  \brief Set action visibility flag.

  Base implementatin does nothing.

  \param actId action ID
  \param place some parent action ID
  \param on visibility state
*/
void QtxActionMgr::setVisible( const int /*actId*/, const int /*place*/, const bool /*on*/ )
{
}

/*!
  \brief Update actions.

  Does nothing if update is disabled.

  \sa isUpdatesEnabled(), internalUpdate()
*/
void QtxActionMgr::update()
{
  if ( !isUpdatesEnabled() )
    return;

  internalUpdate();
  if ( myUpdTimer )
    myUpdTimer->stop();
}

/*!
  \brief Internal update.

  This method is called by update() function and should be redefined in successors.
*/
void QtxActionMgr::internalUpdate()
{
}

/*!
  \brief Generate unique action ID.
  \return new ID
*/
int QtxActionMgr::generateId() const
{
  static int id = -1;
  return --id;
}

/*!
  \brief Check is action with given \a id is enabled.
  \param id action ID
  \return \c true if action is enabled
*/
bool QtxActionMgr::isEnabled( const int id ) const
{
  QAction* a = action( id );
  if ( a )
    return a->isEnabled();
  else
    return false;
}

/*!
  Enable/disable action with given \a id.
  \param id action ID
  \param enable new state
*/
void QtxActionMgr::setEnabled( const int id, const bool enable )
{
  QAction* a = action( id );
  if ( a )
    a->setEnabled( enable );
}

/*!
  \brief Create new separator action.

  If \a own is \c true, that the caller is responsible for the action
  deleting. If \a own is \c false, new separator action is owned by the
  action manager which destroys it on application exit.

  \param own ownership flag
  \return new separator action
*/
QAction* QtxActionMgr::separator( const bool own )
{
  if ( own )
    return new SeparatorAction();

  if ( qtx_separator_actions.isEmpty() )
    qAddPostRoutine( qtxSeparatorActionCleanup );

  SeparatorAction* a = new SeparatorAction();
  qtx_separator_actions.append( a );

  return a;
}

/*!
  \brief Perform delayed update.

  Does nothing if update is disabled.
*/
void QtxActionMgr::triggerUpdate()
{
  if ( !isUpdatesEnabled() )
    return;

  if ( !myUpdTimer )
  {
    myUpdTimer = new QTimer( this );
    myUpdTimer->setSingleShot( true );
    connect( myUpdTimer, SIGNAL( timeout() ), this, SLOT( onUpdateContent() ) );
  }
  myUpdTimer->stop();
  // add timer event to event list
  myUpdTimer->start( 0 );
}

/*!
  \brief Internal content update operation.

  Called automatically by onUpdateContent() when the delayed update
  id triggered. Default implementation does nothing.

  \sa triggerUpdate(), onUpdateContent()
*/
void QtxActionMgr::updateContent()
{}

/*!
  \brief Called when delayed update is performed (via timer event).

  Calls virtual method updateContent() which can customize the 
  content update operation.
*/
void QtxActionMgr::onUpdateContent()
{
  updateContent();
}

/*!
  \class QtxActionMgr::Reader
  \brief Generic actions description files reader class.

  This class is used to read files of some format to create actions 
  and fill an action manager with actions automatically.
*/

/*!
  \brief Reader constructor.

  Does nothing for the moment.
*/
QtxActionMgr::Reader::Reader()
{
}

/*!
  \brief Reader destructor

  Does nothing for the moment.
*/
QtxActionMgr::Reader::~Reader()
{
}

/*!
  \brief Get list of options.
  \return options list
*/
QStringList QtxActionMgr::Reader::options() const
{
  return myOptions.keys();
}

/*!
  \brief Get option value.
  
  If there is no such option the default value is returned.

  \param name option name
  \param def default option value
  \return option value
*/
QString QtxActionMgr::Reader::option( const QString& name, const QString& def ) const
{
  if( myOptions.contains( name ) )
    return myOptions[ name ];
  else
    return def;
}

/*!
  \brief Set option value.
  \param name option name
  \param value new option value
*/
void QtxActionMgr::Reader::setOption( const QString& name, const QString& value )
{
  myOptions[ name ] = value;
}

/*!
  \fn bool QtxActionMgr::Reader::read( const QString& fname, Creator& cr ) const
  \brief Read the file and fill and action manager with actions 
         by using help actions creator. 

  Default implementation is pure virtual.
  
  \param fname XML file name
  \param cr actions creator
  \return \c true in success and \c false in case of error
*/

/*!
  \class QtxActionMgr::XMLReader
  \brief XML file reader.

  This class is used to read files of XML format to create 
  actions and fill an action manager with actions  automatically.
*/

/*!
  \brief XML reader constructor.
  
  \param root root XML tag name
  \param item menu item XML tag name
  \param dir resources directory (containing icons, etc)
*/
QtxActionMgr::XMLReader::XMLReader( const QString& root,
                                    const QString& item,
                                    const QString& dir )
: Reader()
{
  setOption( QString( "root_tag" ),  root );
  setOption( QString( "menu_item" ), item );
  setOption( QString( "icons_dir" ), dir  );
  setOption( QString( "id" ),        QString( "item-id" ) );
  setOption( QString( "pos" ),       QString( "pos-id" ) );
  setOption( QString( "group" ),     QString( "group-id" ) );
  setOption( QString( "label" ),     QString( "label-id" ) );
  setOption( QString( "tooltip" ),   QString( "tooltip-id" ) );
  setOption( QString( "accel" ),     QString( "accel-id" ) );
  setOption( QString( "separator" ), QString( "separator" ) );
  setOption( QString( "icon" ),      QString( "icon-id" ) );
  setOption( QString( "toggle" ),    QString( "toggle-id" ) );
}

/*!
  \brief XML reader destructor.

  Does nothing for the moment.
*/
QtxActionMgr::XMLReader::~XMLReader()
{
}

/*!
  \brief Read the file and fill and action manager with actions 
         by using help actions creator.
  \param fname XML file name
  \param cr actions creator
  \return \c true in success and \c false in case of error
*/
bool QtxActionMgr::XMLReader::read( const QString& fname, Creator& cr ) const
{
  bool res = false;

#ifndef QT_NO_DOM

  QFile file( fname );
  if ( !file.open( QFile::ReadOnly ) )
    return res;

  QDomDocument doc;

  res = doc.setContent( &file );
  file.close();

  if ( !res )
    return res;

  QString root = option( "root_tag" );
  for( QDomNode cur = doc.documentElement(); !cur.isNull(); )
  {
    if( cur.isElement() && isNodeSimilar( cur, root ) )
      read( cur, -1, cr );
    else if( cur.hasChildNodes() )
    {
      cur = cur.firstChild();
      continue;
    }

    while( !cur.isNull() && cur.nextSibling().isNull() )
      cur = cur.parentNode();
    if( !cur.isNull() )
      cur = cur.nextSibling();
  }

#endif

  return res;
}

/*!
  \brief Read XML mode and create an item if requied.
  \param parent_node parent XML file node
  \param parent_id parent action ID
  \param cr actions creator
*/
void QtxActionMgr::XMLReader::read( const QDomNode& parent_node,
                                    const int parent_id,
                                    Creator& cr ) const
{
  if( parent_node.isNull() )
    return;

  QStringList items = option( "menu_item" ).split( "|", QString::SkipEmptyParts );

  const QDomNodeList& children = parent_node.childNodes();
  for( int i=0, n=children.count(); i<n; i++ )
  {
    QDomNode node = children.item( i );
    //QString n = node.nodeName();
    if( node.isElement() /*&& node.hasAttributes()*/ &&
        ( items.contains( node.nodeName() ) || node.nodeName()==option( "separator" ) ) )
    {
      QDomNamedNodeMap map = node.attributes();
      ItemAttributes attrs;

      for( int i=0, n=map.count(); i<n; i++ )
        if( map.item( i ).isAttr() )
        {
          QDomAttr a = map.item( i ).toAttr();
          attrs.insert( a.name(), a.value() );
        }

      int newId = cr.append( node.nodeName(), node.hasChildNodes(), attrs, parent_id );
      if( node.hasChildNodes() )
        read( node, newId, cr );
    }
  }
}

/*!
  \brief Check node name correspondance to some pattern.
  \param node XML file node
  \param pattern node name pattern
  \return \c true if node satisfies pattern
*/
bool QtxActionMgr::XMLReader::isNodeSimilar( const QDomNode& node,
                                             const QString& pattern ) const
{
  if( node.nodeName()==pattern )
    return true;

  QDomDocument temp;
  QString mes;
  temp.setContent( pattern, true, &mes );

  const QDomNamedNodeMap &temp_map = temp.documentElement().attributes(),
                         &cur_map = node.attributes();
  bool ok = temp_map.count()>0;
  for( int i=0, n=temp_map.count(); i<n && ok; i++ )
  {
    QDomAttr a = temp_map.item( i ).toAttr(),
             b = cur_map.namedItem( a.name() ).toAttr();
    ok = !b.isNull() && a.name()==b.name() && a.value()==b.value();
  }

  return ok;
}

/*!
  \class QtxActionMgr::Creator
  \brief Generic actions creator class.

  Used by Reader to create actions and fill in the action 
  manager with the actions.
*/

/*!
  \brief Get integer attribute value from the attribute map.

  Returns default value if the attribute is not found.

  \param attrs attributes map
  \param name attribute name
  \param def default attribute value
  \return attribute value
*/
int QtxActionMgr::Creator::intValue( const ItemAttributes& attrs,
                                     const QString& name, int def )
{
  if( attrs.contains( name ) )
  {
    bool ok;
    int res = attrs[ name ].toInt( &ok );
    if( ok )
      return res;
  }
  return def;
}

/*!
  \brief Get string attribute value from the attribute map.

  Returns default value if the attribute is not found.

  \param attrs attributes map
  \param name attribute name
  \param def default attribute value
  \return attribute value
*/
QString QtxActionMgr::Creator::strValue( const ItemAttributes& attrs,
                                         const QString& name,
                                         const QString& def  )
{
  if( attrs.contains( name ) )
    return attrs[ name ];
  else
    return def;
}

/*!
  \brief Creator constructor.
  \param r action reader
*/
QtxActionMgr::Creator::Creator( QtxActionMgr::Reader* r )
: myReader( r )
{
}

/*!
  \brief Creator destructor.

  Does nothing for the moment.
*/
QtxActionMgr::Creator::~Creator()
{
}

/*!
  \brief Get actions reader.
  \return actions reader
*/
QtxActionMgr::Reader* QtxActionMgr::Creator::reader() const
{
  return myReader;
}

/*!
  \brief Connect action to some specific slot(s)

  Default implementation does nothing.

  \param a action
*/
void QtxActionMgr::Creator::connect( QAction* /*a*/ ) const
{
}

/*!
  \brief Load pixmap from the file.
  \param fname file name
  \param pix uaed to return pixmap
  \return \c true if pixmap is loaded successfully and \c false in case of error
*/
bool QtxActionMgr::Creator::loadPixmap( const QString& fname, QPixmap& pix ) const
{
  if( !reader() )
    return false;

  QStringList dirlist = reader()->option( "icons_dir", "." ).split( ";", QString::SkipEmptyParts );
  QStringList::const_iterator anIt = dirlist.begin(),
                              aLast = dirlist.end();
  bool res = false;
  for( ; anIt!=aLast && !res; anIt++ )
    res = pix.load( Qtx::addSlash( *anIt ) + fname );

  return res;
}

/*!
  \fn int QtxActionMgr::Creator::append( const QString& tag, 
                                         const bool subMenu, 
                                         const ItemAttributes& attr,
                                         const int pId )
  \brief Create (and probably append to the action manager) new action.

  Default implementation is pure virtual.
  
  \param tag item tag name
  \param subMenu \c true if this item is submenu
  \param attr attributes map
  \param pId parent action ID
  \return item (for example action) ID
*/
