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

static const char* browse_icon[] = {
"16 16 5 1",
"  c none",
". c #ffff00",
"# c #848200",
"a c #ffffff",
"b c #000000",
"                ",
"          bbb   ",
"         b   b b",
"              bb",
"  bbb        bbb",
" ba.abbbbbbb    ",
" b.a.a.a.a.b    ",
" ba.a.a.a.ab    ",
" b.a.abbbbbbbbbb",
" ba.ab#########b",
" b.ab#########b ",
" bab#########b  ",
" bb#########b   ",
" bbbbbbbbbbb    ",
"                ",
"                "
};

QtxPathEdit::QtxPathEdit( const Qtx::PathType type, QWidget* parent )
: QFrame( parent ),
myType( type )
{
  initialize();
}

QtxPathEdit::QtxPathEdit( QWidget* parent )
: QFrame( parent ),
myType( Qtx::PT_OpenFile )
{
  initialize();
}

QtxPathEdit::~QtxPathEdit()
{
}

Qtx::PathType QtxPathEdit::pathType() const
{
  return myType;
}

void QtxPathEdit::setPathType( const Qtx::PathType type )
{
  if ( myType == type )
    return;

  myType = type;
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

QString QtxPathEdit::pathFilter() const
{
  return myFilter;
}

void QtxPathEdit::setPathFilter( const QString& f )
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
  switch ( pathType() )
  {
  case Qtx::PT_OpenFile:
    path = QFileDialog::getOpenFileName( myPath, QString(), initial, pathFilter() );
    break;
  case Qtx::PT_SaveFile:
    path = QFileDialog::getSaveFileName( myPath, QString(), initial, pathFilter() );
    break;
  case Qtx::PT_Directory:
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
  browse->setIcon( QPixmap( browse_icon ) );
  base->addWidget( browse );

  connect( browse, SIGNAL( clicked( bool ) ), this, SLOT( onBrowse( bool ) ) );

  setFocusProxy( myPath );

  updateState();
}

void QtxPathEdit::updateState()
{
  myPath->setCompleter( Qtx::pathCompleter( pathType(), pathFilter() ) );
}
