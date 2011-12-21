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
// File:      QtxActionMgrIO.cxx
// Author:    Sergey TELKOV

#include "QtxActionMgrIO.h"

#include "QtxResourceMgr.h"

QtxActionMgrIO::QtxActionMgrIO()
{
}

QtxActionMgrIO::~QtxActionMgrIO()
{
}

/*!
  \brief Get the list of options.
  \return options list
*/
QStringList QtxActionMgrIO::options() const
{
  return myOptions.keys();
}

/*!
  \brief Get option value.

  If there is no such option the default value (\a def) is returned.

  \param name option name
  \param def default option value
  \return option value
*/
QString QtxActionMgrIO::option( const QString& name, const QString& def ) const
{
  return myOptions.contains( name ) ? myOptions[name] : def;
}

/*!
  \brief Set option value.
  \param name option name
  \param value new option value
*/
void QtxActionMgrIO::setOption( const QString& name, const QString& value )
{
  myOptions.insert( name, value );
}



QtxActionMgrResIO::QtxActionMgrResIO( QtxResourceMgr* resMgr )
  : QtxActionMgrIO(),
    myResMgr( resMgr )
{
}

QtxActionMgrResIO::~QtxActionMgrResIO()
{
}

QtxResourceMgr* QtxActionMgrResIO::resourceMgr() const
{
  return myResMgr;
}

bool QtxActionMgrResIO::load( const QString& ref, QMap<QtxActionMgrId, Node>& nodeMap ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( !resMgr || ref.isEmpty() )
    return false;

  QStringList paramList = resMgr->parameters( ref );
  for ( QStringList::iterator it = paramList.begin(); it != paramList.end(); ++it ) {
    QString data;
    QString param = *it;
    QString value = resMgr->stringValue( ref, param );

    int idx = param.indexOf( "." );
    if ( idx != -1 ) {
      data = param.mid( idx + 1 );
      param = param.mid( 0, idx );
    }
    QtxActionMgrId id = nodeId( param );
    if ( id.isNull() )
      continue;

    if ( !nodeMap.contains( id ) )
      nodeMap.insert( id, Node( id ) );

    Node& ioNode = nodeMap[id];
    if ( data.isEmpty() ) {
      QList<QtxActionMgrId> childList;
      QStringList lst = value.split( "," );
      for ( QStringList::iterator it = lst.begin(); it != lst.end(); ++it ) {
        QtxActionMgrId childId = nodeId( *it );
        if ( !childId.isNull() )
          childList.append( childId );
      }
      ioNode.setChildren( childList );
    }
    else
      ioNode.setData( data, value );
  }

  return true;
}

bool QtxActionMgrResIO::save( const QString& ref, const QMap<QtxActionMgrId, Node>& nodeMap ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( !resMgr || ref.isEmpty() )
    return false;

  resMgr->remove( ref );

  for ( QMap<QtxActionMgrId, Node>::const_iterator it = nodeMap.begin(); it != nodeMap.end(); ++it ) {
    const Node& ioNode = *it;
    QString idName = nodeId( ioNode.id() );

    QList<QtxActionMgrId> childList = ioNode.children();
    if ( !childList.isEmpty() ) {
      QStringList childStrList;
      for ( QList<QtxActionMgrId>::iterator iter = childList.begin(); iter != childList.end(); ++iter )
        childStrList.append( nodeId( *iter ) );
      resMgr->setValue( ref, idName, childStrList.join( "," ) );
    }

    QStringList data = ioNode.data();
    for ( QStringList::iterator itr = data.begin(); itr != data.end(); ++itr ) {
      QString param = *itr;
      if ( param.isEmpty() )
        continue;

      resMgr->setValue( ref, QString( "%1.%2" ).arg( idName ).arg( param ), ioNode.data( param ).toString() );
    }
  }

  return true;
}

QtxActionMgrId QtxActionMgrResIO::nodeId( const QString& str ) const
{
  int id = -1;
  QString ctx;
  QString txt = str.trimmed();
  if ( !txt.isEmpty() ) {
    int idx = txt.indexOf( ":" );
    if ( idx != -1 ) {
      ctx = txt.mid( idx + 1 );
      txt = txt.mid( 0, idx );
    }
    bool ok = false;
    id = txt.toInt( &ok );
    if ( !ok )
      id = -1;
  }

  return QtxActionMgrId( id, id == -1 ? QString() : ctx );
}

QString QtxActionMgrResIO::nodeId( const QtxActionMgrId& id ) const
{
  QString res;
  if ( !id.isNull() ) {
    res = QString::number( id.id() );
    if ( !id.context().isEmpty() )
      res += QString( ":") + id.context();
  }
  return res;
}
