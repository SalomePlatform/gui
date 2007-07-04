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
// File:      QtxPathEdit.cxx
// Author:    Sergey TELKOV

#include "QtxPathEdit.h"

#include <QLayout>
#include <QDirModel>
#include <QLineEdit>
#include <QCompleter>
#include <QToolButton>
#include <QFileDialog>
#include <QRegExpValidator>

QtxPathEdit::QtxPathEdit( const int mode, QWidget* parent )
: QFrame( parent ),
myMode( mode )
{
  initialize();
}

QtxPathEdit::QtxPathEdit( QWidget* parent )
: QFrame( parent ),
myMode( OpenFile )
{
  initialize();
}

QtxPathEdit::~QtxPathEdit()
{
}

int QtxPathEdit::mode() const
{
  return myMode;
}

void QtxPathEdit::setMode( const int mode )
{
  if ( myMode == mode )
    return;

  myMode = mode;
  updateState();
}

QString QtxPathEdit::path() const
{
  return myPath->text();
}

void QtxPathEdit::setPath( const QString& txt )
{
  myPath->setText( txt );
}

QString QtxPathEdit::filter() const
{
  return myFilter;
}

void QtxPathEdit::setFilter( const QString& f )
{
  if ( myFilter == f )
    return;

  myFilter = f;
  updateState();
}

void QtxPathEdit::onBrowse( bool )
{
  QString path;
  QString initial = QFileInfo( myPath->text() ).path();
  switch ( mode() )
  {
  case OpenFile:
    path = QFileDialog::getOpenFileName( myPath, QString(), initial, filter() );
    break;
  case SaveFile:
    path = QFileDialog::getSaveFileName( myPath, QString(), initial, filter() );
    break;
  case Directory:
    path = QFileDialog::getExistingDirectory( myPath, QString(), initial );
    break;
  }

  if ( !path.isEmpty() )
    myPath->setText( QDir::convertSeparators( path ) ); 

  myPath->setFocus();
}

QLineEdit* QtxPathEdit::lineEdit() const
{
  return myPath;
}

void QtxPathEdit::initialize()
{
  QHBoxLayout* base = new QHBoxLayout( this );
  base->setMargin( 0 );
  base->setSpacing( 5 );

  base->addWidget( myPath = new QLineEdit( this ) );
  myPath->setValidator( new QRegExpValidator( QRegExp( "^([\\w/]{2}|[A-Z]:)[^:;\\*\\?]*[\\w\\\\/\\.]$" ), myPath ) );

  QToolButton* browse = new QToolButton( this );
  browse->setText( "..." );
  base->addWidget( browse );

  connect( browse, SIGNAL( clicked( bool ) ), this, SLOT( onBrowse( bool ) ) );

  setFocusProxy( myPath );

  updateState();
}

void QtxPathEdit::updateState()
{
  QStringList extList;
  QStringList filterList = filter().split( ";;" );
  for ( QStringList::const_iterator it = filterList.begin(); it != filterList.end(); ++it )
  {
    QRegExp rx( "[\\s\\w,;]*\\(?\\*\\.([\\w]+)\\)?[\\d\\s\\w]*" );
    int index = 0;
    while ( ( index = rx.indexIn( *it, index ) ) != -1 )
    {
      extList.append( QString( "*.%1" ).arg( rx.cap( 1 ) ) );
      index += rx.matchedLength();
    }
  }

  QDir::Filters filters = 0;
  switch ( mode() )
  {
  case OpenFile:
  case SaveFile:
    filters = QDir::AllEntries | QDir::AllDirs | QDir::NoDotAndDotDot;
    break;
  case Directory:
    filters = QDir::Drives | QDir::Dirs | QDir::NoDotAndDotDot;
    break;
  }

  QDirModel* dm = new QDirModel( extList, filters, QDir::Unsorted );
  QCompleter* cmp = new QCompleter( dm, myPath );
  dm->setParent( cmp );

  myPath->setCompleter( cmp );
}
