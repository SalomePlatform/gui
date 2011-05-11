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

  The color preference item is represented as the colored button with
  assocoiated popup menu whihc is called when the user presses the small 
  arrow button near it. The popup menu allows selecting of the color
  from the predefined set. In addition it contains the button which
  invokes standard "Select color" dialog box.

  Initial color value can be set with setColor() method. Chosen color
  can be retrieved with the color() method.
*/

/*!
  \brief Constructor.
  \param parent parent widget
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
      
      btn->installEventFilter( this );

      connect( btn, SIGNAL( clicked( bool ) ), this, SLOT( onToggled( bool ) ) );

      updateButton( btn );
    }
  }

  myOthers = new QToolButton( pm );
  myOthers->setText( tr( "Other colors..." ) );
  myOthers->setAutoRaise( true );
  myOthers->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  grid->addWidget( myOthers, grid->rowCount(), 0, 1, grid->columnCount() );
  connect( myOthers, SIGNAL( clicked( bool ) ), this, SLOT( onDialogClicked( bool ) ) );

  myOthers->installEventFilter( this );

  setMenu( pm );

  connect( this, SIGNAL( clicked( bool ) ), this, SLOT( onClicked( bool ) ) );
  connect( pm,   SIGNAL( aboutToShow() ),   this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Destructor.
*/
QtxColorButton::~QtxColorButton()
{
}

/*!
  \brief Get currently selected color.

  Returns null QColor if no color is selected.

  \return selected color
  \sa setColor()
*/
QColor QtxColorButton::color() const
{
  return myColors.contains( this ) ? myColors[this] : QColor();
}

/*!
  \brief Set color.
  \param c color to be set as current
  \sa color()
*/
void QtxColorButton::setColor( const QColor& c )
{
  myColors.insert( this, c );
  updateState();
  update();
  updateButton( this );
}

/*!
  \brief Filter events for the child widgets.
  \param o event receiver object
  \param e event
  \return \c true if the event should be filtered
*/
bool QtxColorButton::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::Leave )
    updateButton( qobject_cast<QToolButton*>( o ) );
  return QToolButton::eventFilter( o, e );
}

bool QtxColorButton::isColorChooserEnabled() const
{
  return myOthers->isVisibleTo( myOthers->parentWidget() );
}

void QtxColorButton::setColorChooserEnabled( bool on )
{
  myOthers->setVisible( on );
}

/*!
  \brief Called when the popup menu is about to show.

  Updates the menu and child widgets state.
*/
void QtxColorButton::onAboutToShow()
{
  updateState();
}

/*!
  \brief Called when the button is clicked by the user.

  Emits the signal clicked( QColor ).

  \param on button state (not used)
*/
void QtxColorButton::onClicked( bool /*on*/ )
{
  emit clicked( color() );
}

/*!
  \brief Called when any color selection button from popup menu
  is clicked.

  Changes the currently selected color and emits the signal
  changed( QColor ).

  \param on button state
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
  \brief Called the "Other colors" child button from popup menu
  is clicked.

  Invokes standard "Select color" dialog box allowing user to select
  custom color. If the current color is changed by the user, emits 
  the signal changed( QColor ).

  \param on (not used)
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
  \brief Customize paint event for the widget.
  \param e paint event
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
  pix.fill( palette().color( backgroundRole() ) );
  drawColor( &pix, color() );

  QPainter p( this );
  p.drawPixmap( r, pix );
  p.end();
}

/*!
  \brief Update widget state.
*/
void QtxColorButton::updateState()
{
  QList<QToolButton*> bList = qFindChildren<QToolButton*>( menu() );
  for ( QList<QToolButton*>::iterator cit = bList.begin(); cit != bList.end(); ++cit )
    updateButton( *cit );
}

/*!
  \brief Update child button state.
  \param btn child button
*/
void QtxColorButton::updateButton( QToolButton* btn )
{
  QColor c = color().toRgb();
  bool block = btn->signalsBlocked();
  btn->blockSignals( true );
  btn->setChecked( false );
  if ( myColors.contains( btn ) ) {
    btn->setIcon( buttonIcon( myColors[btn] ) );
    btn->setChecked( myColors[btn].toRgb() == c );
  }
  btn->setDown( false );
  btn->blockSignals( block );
}

/*!
  \brief Generate (if necessary) or get the icon for the button.
  \param c color to be used for the icon
  \return icon pixmap for the button
*/
QPixmap QtxColorButton::buttonIcon( const QColor& c ) const
{
  static QMap<int, QPixmap> pixMap;

  if ( pixMap.contains( c.rgb() ) )
    return pixMap[c.rgb()];

  QPixmap pix( 16, 16 );

  QColor bg = Qt::white;
  if ( bg == c )
    bg = Qt::gray;
  pix.fill( bg );

  drawColor( &pix, c );

  pix.setMask( pix.createHeuristicMask() );

  pixMap.insert( c.rgb(), pix );

  return pix;
}

/*!
  \brief Draw pixmap.
  \param pd paint device
  \param c color
  \param m margin
*/
void QtxColorButton::drawColor( QPaintDevice* pd, const QColor& c, const int m ) const
{
  if ( !pd )
    return;

  QPainter p( pd );
  p.setPen( Qt::black );
  p.fillRect( m, m, pd->width() - 2 * m - 1, pd->height() - 2 * m - 1, QBrush( c ) );
  p.drawRect( m, m, pd->width() - 2 * m - 1, pd->height() - 2 * m - 1 );
  p.end();
}

/*!
  \brief Get predefined list of colors to be used in the popup menu.
  \return list of colors
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

/*!
  \fn void QtxColorButton::clicked( QColor color );
  \brief This signal is emitted when the widget button is clicked by 
         the user.
  \param color current color
*/

/*!
  \fn void QtxColorButton::changed( QColor color );
  \brief This signal is emitted when the current color is changed.
  \param color new current color
*/
