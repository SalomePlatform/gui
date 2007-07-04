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
// File:      QtxFontEdit.cxx
// Author:    Sergey TELKOV

#include "QtxFontEdit.h"

#include "QtxComboBox.h"

#include <QLayout>
#include <QToolButton>
#include <QFontDialog>
#include <QFontDatabase>
#include <QFontComboBox>

QtxFontEdit::QtxFontEdit( const int feat, QWidget* parent )
: QFrame( parent ),
myFeatures( feat )
{
  initialize();
}

QtxFontEdit::QtxFontEdit( QWidget* parent )
: QFrame( parent ),
myFeatures( All )
{
  initialize();
}

QtxFontEdit::~QtxFontEdit()
{
}

int QtxFontEdit::features() const
{
  return myFeatures;
}

void QtxFontEdit::setFeatures( const int f )
{
  if ( myFeatures == f )
    return;

  myFeatures = f;
  updateState();
}

QFont QtxFontEdit::currentFont() const
{
  QFont fnt( fontFamily(), fontSize() );

  int script = fontScripting();
  fnt.setBold( script & Bold );
  fnt.setItalic( script & Italic );
  fnt.setUnderline( script & Underline );

  return fnt;
}

void QtxFontEdit::setCurrentFont( const QFont& fnt )
{
  setFontFamily( fnt.family() );
  setFontSize( fnt.pointSize() );
  setFontScripting( ( fnt.bold() ? Bold : 0 ) |
                    ( fnt.italic() ? Italic : 0 ) |
                    ( fnt.underline() ? Underline : 0 ) );
}

QString QtxFontEdit::fontFamily() const
{
  return myFamily->currentFont().family();
}

int QtxFontEdit::fontSize() const
{
  bool ok;
  int pSize = mySize->currentText().toInt( &ok );
  return ok ? pSize : 0;
}

int QtxFontEdit::fontScripting() const
{
  return ( myB->isChecked() ? Bold : 0 ) |
         ( myI->isChecked() ? Italic : 0 ) |
         ( myU->isChecked() ? Underline : 0 );
}

void QtxFontEdit::setFontFamily( const QString& fam )
{
  myFamily->setCurrentFont( QFont( fam ) );
  onFontChanged( myFamily->currentFont() );  
}

void QtxFontEdit::setFontSize( const int s )
{
  if ( s <= 0 )
    return;

  int idx = mySize->findText( QString::number( s ) );
  if ( idx != -1 )
    mySize->setCurrentIndex( idx );
  else if ( mySize->isEditable() )
    mySize->setEditText( QString::number( s ) );
}

void QtxFontEdit::setFontScripting( const int script )
{
  myB->setChecked( script & Bold );
  myI->setChecked( script & Italic );
  myU->setChecked( script & Underline );
}

void QtxFontEdit::updateState()
{
  int feat = features();

  myFamily->setVisible( feat & Family );
  mySize->setVisible( feat & Size );
  myB->setVisible( feat & Bold );
  myI->setVisible( feat & Italic );
  myU->setVisible( feat & Underline );
  myPreview->setVisible( feat & Preview );

  mySize->setEditable( feat & UserSize );
}

void QtxFontEdit::onFontChanged( const QFont& )
{
  int s = fontSize();
  mySize->clear();

  QList<int> szList = QFontDatabase().pointSizes( fontFamily() );
  QStringList sizes;
  for ( QList<int>::const_iterator it = szList.begin(); it != szList.end(); ++it )
    sizes.append( QString::number( *it ) );
  mySize->addItems( sizes );

  setFontSize( s );
}

void QtxFontEdit::onPreview( bool )
{
  bool ok;
  QFont fnt = QFontDialog::getFont( &ok, currentFont() );

  if ( ok )
    setCurrentFont( fnt );
}

void QtxFontEdit::initialize()
{
  QHBoxLayout* base = new QHBoxLayout( this );
  base->setMargin( 0 );
  base->setSpacing( 5 );

  base->addWidget( myFamily = new QFontComboBox( this ) );
  base->addWidget( mySize = new QtxComboBox( this ) );
  mySize->setInsertPolicy( QComboBox::NoInsert );
  mySize->setValidator( new QIntValidator( 1, 250, mySize ) );

  base->addWidget( myB = new QToolButton( this ) );
  myB->setText( tr( "B" ) );
  myB->setCheckable( true );

  base->addWidget( myI = new QToolButton( this ) );
  myI->setText( tr( "I" ) );
  myI->setCheckable( true );

  base->addWidget( myU = new QToolButton( this ) );
  myU->setText( tr( "U" ) );
  myU->setCheckable( true );

  base->addWidget( myPreview = new QToolButton( this ) );
  myPreview->setText( "..." );

  myFamily->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );

  connect( myPreview, SIGNAL( clicked( bool ) ), this, SLOT( onPreview( bool ) ) );
  connect( myFamily, SIGNAL( currentFontChanged( const QFont& ) ), this, SLOT( onFontChanged( const QFont& ) ) );

  updateState();
  onFontChanged( currentFont() );
}
