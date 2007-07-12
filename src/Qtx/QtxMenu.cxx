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
// File:      QtxMenu.cxx
// Author:    Sergey TELKOV

#include "QtxMenu.h"

#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QTextDocument>
#include <QWidgetAction>
#include <QLinearGradient>
#include <QAbstractTextDocumentLayout>

/*!
    Class: QtxMenu::Title [Internal]
    Descr: Menu item for popup title.
*/

class QtxMenu::Title : public QWidget
{
public:
  Title( QWidget* = 0 );
  virtual ~Title();

  QIcon            icon() const;
  void             setIcon( const QIcon& );

  QString          text() const;
  void             setText( const QString& );

  Qt::Alignment    alignment() const;
  void             setAlignment( const Qt::Alignment );

  virtual QSize    sizeHint() const;
  virtual QSize    minimumSizeHint() const;

protected:
  virtual void     paintEvent( QPaintEvent* );

private:
  QIcon            myIcon;
  QString          myText;
  Qt::Alignment    myAlignment;
};

/*!
  Constructor
*/
QtxMenu::Title::Title( QWidget* parent )
: QWidget( parent ),
myAlignment( 0 )
{
}

/*!
  Destructor
*/
QtxMenu::Title::~Title()
{
}

QIcon QtxMenu::Title::icon() const
{
  return myIcon;
}

void QtxMenu::Title::setIcon( const QIcon& ico )
{
  myIcon = ico;
}

QString QtxMenu::Title::text() const
{
  return myText;
}

void QtxMenu::Title::setText( const QString& txt )
{
  myText = txt;
}

Qt::Alignment QtxMenu::Title::alignment() const
{
  return myAlignment;
}

void QtxMenu::Title::setAlignment( const Qt::Alignment a )
{
  myAlignment = a;
}

QSize QtxMenu::Title::sizeHint() const
{
  int m = 5;
  QTextDocument doc;
  doc.setHtml( text() );

  QSize sz = icon().isNull() ? QSize( 0, 0 ) : icon().actualSize( QSize( 16, 16 ) );
  sz.setWidth( 2 * m + sz.width() + doc.size().width() );
  sz.setHeight( 2 * m + qMax( sz.height(), (int)doc.size().height() ) );
  return sz;
}

QSize QtxMenu::Title::minimumSizeHint() const
{
  return sizeHint();
}

void QtxMenu::Title::paintEvent( QPaintEvent* )
{
  int m = 5;
  QIcon ico = icon();
  QString txt = text();
  Qt::Alignment align = alignment();

  QRect base = rect();
  base.setTop( base.top() + 1 );
  base.setLeft( base.left() + 1 );
  base.setRight( base.right() -1 );
  base.setBottom( base.bottom() - 1 );

  QTextDocument doc;
  doc.setHtml( txt );

  QSize isz = ico.isNull() ? QSize( 0, 0 ) : ico.actualSize( QSize( 16, 16 ) );
  QSize sz( doc.size().width(), doc.size().height() );

  QPainter p( this );
  QAbstractTextDocumentLayout::PaintContext ctx;
  ctx.palette.setColor( QPalette::Text, palette().color( QPalette::Light ) );

  QLinearGradient linearGrad( base.topLeft(), base.topRight() );
  linearGrad.setColorAt( 0, palette().color( QPalette::Highlight ) );
  linearGrad.setColorAt( 1, palette().color( QPalette::Window ) );

  p.fillRect( base, linearGrad );

  QPoint start = base.topLeft() + QPoint( m, m );
  if ( align & Qt::AlignLeft )
    start.setX( base.left() + m );
  else if ( align & Qt::AlignRight )
    start.setX( base.right() - m - isz.width() - sz.width() );
  else if ( align & Qt::AlignHCenter )
    start.setX( base.left() + ( base.width() - isz.width() - sz.width() ) / 2 );

  if ( align & Qt::AlignTop )
    start.setY( base.top() + m );
  else if ( align & Qt::AlignBottom )
    start.setY( base.bottom() - m - qMax( isz.height(), - sz.height() ) );
  else if ( align & Qt::AlignVCenter )
    start.setY( base.top() + ( base.height() - qMax( isz.height(), sz.height() ) ) / 2 );

  if ( !ico.isNull() )
  {
    ico.paint( &p, QRect( start, isz ) );
    start.setX( start.x() + isz.width() );
  }

  p.save();
  p.translate( start );
  doc.documentLayout()->draw( &p, ctx );
  p.restore();
}

/*!
  Constructor
*/
QtxMenu::QtxMenu( QWidget* parent )
: QMenu( parent ),
myMode( TitleAuto )
{
  myTitle = new Title( this );
  myAction = new QWidgetAction( this );
  myAction->setDefaultWidget( myTitle );
}

/*!
  Destructor
*/
QtxMenu::~QtxMenu()
{
}

QString QtxMenu::titleText() const
{
  return myTitle->text();
}

/*!
  \return popup menu icon
*/
QIcon QtxMenu::titleIcon() const
{
  return myTitle->icon();
}

/*!
  \return popup menu title policy
*/
QtxMenu::TitleMode QtxMenu::titleMode() const
{
  return myMode;
}

/*!
  \return popup menu title alignment
*/
Qt::Alignment QtxMenu::titleAlignment() const
{
  return myTitle->alignment();
}

/*!
  Changes title text
  \param txt - new text
*/
void QtxMenu::setTitleText( const QString& txt )
{
  if ( titleText() == txt )
    return;

  myTitle->setText( txt );

  updateTitle();
}

/*!
  Changes title icon
  \param icon - new icon
*/
void QtxMenu::setTitleIcon( const QIcon& ico )
{
  myTitle->setIcon( ico );

  updateTitle();
}

/*!
  Changes title mode
  \param p - new policy
*/
void QtxMenu::setTitleMode( const QtxMenu::TitleMode m )
{
  if ( myMode == m )
    return;

  myMode = m;

  updateTitle();
}

/*!
  Changes title alignment
  \param a - new alignment
*/
void QtxMenu::setTitleAlignment( const Qt::Alignment a )
{
  if ( titleAlignment() == a )
    return;

  myTitle->setAlignment( a );

  updateTitle();
}

/*!
  Shows/hide menu
*/
void QtxMenu::setVisible( bool on )
{
  if ( on )
    insertTitle();

  QMenu::setVisible( on );

  if ( !on )
    removeTitle();
}

/*!
  Inserts title item to popup menu
*/
void QtxMenu::insertTitle()
{
  if ( titleMode() == TitleOff || ( titleMode() == TitleAuto && titleText().trimmed().isEmpty() ) )
    return;

  if ( actions().isEmpty() )
    addAction( myAction );
  else
    insertAction( actions().first(), myAction );
}

/*!
  Removes title item from popup menu
*/
void QtxMenu::removeTitle()
{
  if ( actions().contains( myAction ) )
    removeAction( myAction );
}

/*!
  Updates title item
*/
void QtxMenu::updateTitle()
{
  if ( !actions().contains( myAction ) )
    return;

  removeTitle();
  insertTitle();
}
