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
// File:      QtxColorButton.cxx
// Author:    Sergey TELKOV

#include "QtxColorButton.h"

#include <QMenu>
#include <QStyle>
#include <QLayout>
#include <QBitmap>
#include <QPainter>
#include <QPaintEvent>
#include <QColorDialog>
#include <QStyleOptionToolButton>

/*!
  \class QtxColorButton
  \brief The QtxColorButton class implements a widget for color
  preference items editing.

  The color preference item is represented as the colored button,
  clicking on which invokes the standard "Select color" dialog box.

  Initial color value can be set with setColor() method. Currently
  chosen color can be retrieved with color() method.
*/

/*!
  \brief 
*/
QtxColorButton::QtxColorButton( QWidget* parent )
: QToolButton( parent )
{
  setCheckable( false );
  setPopupMode( MenuButtonPopup );

  QMenu* pm = new QMenu( this );
  QGridLayout* grid = new QGridLayout( pm );
  grid->setMargin( 5 );
  grid->setSpacing( 0 );

  QList<QColor> cList = colorsList();
  int w = 8;
  int h = cList.count() / w;

  for ( int y = 0; y < h; y++ )
  {
    for ( int x = 0; x < w; x++ )
    {
      QColor c = cList.at( x * h + y ).toRgb();
      QToolButton* btn = new QToolButton( pm );
      btn->setAutoRaise( true );
      btn->setCheckable( true );
      myColors.insert( btn, c );
      grid->addWidget( btn, y, x );

      connect( btn, SIGNAL( toggled( bool ) ), this, SLOT( onToggled( bool ) ) );

      updateButton( btn );
    }
  }

  QToolButton* other = new QToolButton( pm );
  other->setText( tr( "Other colors..." ) );
  other->setAutoRaise( true );
  other->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  grid->addWidget( other, grid->rowCount(), 0, 1, grid->columnCount() );
  connect( other, SIGNAL( clicked( bool ) ), this, SLOT( onDialogClicked( bool ) ) );

  setMenu( pm );

  connect( this, SIGNAL( clicked( bool ) ), this, SLOT( onClicked( bool ) ) );
}

/*!
  \brief 
*/
QtxColorButton::~QtxColorButton()
{
}

/*!
  \brief 
*/
QColor QtxColorButton::color() const
{
  return myColors.contains( this ) ? myColors[this] : QColor();
}

/*!
  \brief 
*/
void QtxColorButton::setColor( const QColor& c )
{
  myColors.insert( this, c );
  updateButton( this );
  updateState();
}

/*!
  \brief 
*/
void QtxColorButton::onClicked( bool )
{
  emit clicked( color() );
}

/*!
  \brief 
*/
void QtxColorButton::onToggled( bool on )
{
  const QToolButton* tb = ::qobject_cast<QToolButton*>( sender() );
  if ( !tb )
    return;

  QColor old = color();

  if ( on && myColors.contains( tb ) )
  {
    myColors.insert( this, myColors[tb] );
    updateButton( this );
  }

  if ( menu() )
    menu()->hide();

  updateState();

  if ( old != color() )
    emit changed( color() );
}

/*!
  \brief 
*/
void QtxColorButton::onDialogClicked( bool )
{
  QColor c = QColorDialog::getColor( color(), this );
  if ( !c.isValid() )
    return;

  QColor old = color();

  setColor( c );

  if ( old != color() )
    emit changed( color() );
}

/*!
  \brief 
*/
void QtxColorButton::paintEvent( QPaintEvent* e )
{
  QToolButton::paintEvent( e );

  if ( !color().isValid() )
    return;

  QStyleOptionToolButton opt;
  opt.initFrom( this );
  opt.text = text();
  opt.icon = icon();
  opt.features = QStyleOptionToolButton::Menu;

  QRect r = style()->subControlRect( QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButton );
  r.setTopLeft( r.topLeft() + QPoint( 2, 2 ) );
  r.setBottomRight( r.bottomRight() - QPoint( 2, 2 ) );

  QPixmap pix( r.size() );
  drawColor( &pix, color() );

  QPainter p( this );
  p.drawPixmap( r, pix );
  p.end();
}

/*!
  \brief 
*/
void QtxColorButton::updateState()
{
  QColor c = color().toRgb();
  for ( ColorMap::iterator it = myColors.begin(); it != myColors.end(); ++it )
  {
    QColor bc = it.value().toRgb();
    QToolButton* b = (QToolButton*)it.key();
    bool block = b->signalsBlocked();
    b->blockSignals( true );

    if ( bc == c )
      b->setChecked( true );
    else
      b->setChecked( false );
    b->blockSignals( block );
  }
}

/*!
  \brief 
*/
void QtxColorButton::updateButton( QToolButton* btn )
{
  btn->setIcon( buttonIcon( myColors[btn] ) );
}

QPixmap QtxColorButton::buttonIcon( const QColor& c ) const
{
  static QMap<int, QPixmap> pixMap;

  if ( pixMap.contains( c.rgb() ) )
    return pixMap[c.rgb()];

  QPixmap pix( 16, 16 );

  drawColor( &pix, c );

  pix.setMask( pix.createHeuristicMask() );

  pixMap.insert( c.rgb(), pix );

  return pix;
}

/*!
  \brief 
*/
void QtxColorButton::drawColor( QPaintDevice* pd, const QColor& c, const int m ) const
{
  if ( !pd )
    return;

  QPainter p( pd );
  p.setPen( Qt::black );
  p.fillRect( m, m, pd->width() - 2 * m, pd->height() - 2 * m, QBrush( c ) );
  p.drawRect( m, m, pd->width() - 2 * m - 1, pd->height() - 2 * m - 1 );
  p.end();
}

/*!
  \brief 
*/
QList<QColor> QtxColorButton::colorsList() const
{
  QList<QColor> lst;

  for ( int g = 0; g < 4; g++ )
  {
    for ( int r = 0; r < 4; r++ )
    {
      for ( int b = 0; b < 3; b++ )
        lst.append( QColor( qRgb( r * 255 / 3, g * 255 / 3, b * 255 / 2 ) ) );
    }
  }
  return lst;
}
