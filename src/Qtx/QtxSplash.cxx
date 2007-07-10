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
// File   : QtxSplash.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "QtxSplash.h"

#include <QApplication>
#include <QPainter>
#include <QMessageBox>
#include <QDesktopWidget>

/*!
  \class ProgressEvent
  \internal
  \brief Progress change custom event.
*/

class ProgressEvent: public QEvent
{
public:
  /*!
    \brief Constructor.
    \param msg progress message
    \param progress current progress (for example, in %)
  */
  ProgressEvent( const QString& msg, const int progress = 0 )
    : QEvent( (QEvent::Type)id() ),
      myMessage( msg ),
      myProgress( progress )
  {}
  /*!
    \brief Get progress message.
    \return message
  */
  QString    message()  const { return myMessage; } 
  /*!
    \brief Get current progress.
    \return current progress
  */
  int        progress() const { return myProgress; }
  /*!
    \brief Get message identifier.
    \return custom message ID
  */
  static int id()             { return QEvent::User + 10; }

private:
  QString myMessage;
  int     myProgress;
};

/*!
  \class QtxSplash
  \brief The QtxSplash widget provides a splash screen that can be shown during application startup..

  A splash screen is a widget that is usually displayed when an application is being started. 
  Splash screens are often used for applications that have long start up times to provide 
  the user with feedback that the application is loading.

  Only one instance of the QtxSplash widget can be created. To access to the splash screen widget,
  use static method QtxSplash::splash(), which creates and instance of the QtxSplash widget if
  necessary and returns pointer to it. You should not destroy yhis instance - it is done automatically
  after application main window is shown. Just use methods finish() to make splash screen wait untill 
  main window is shown.

  The splash screen appears in the center of the screen. The most common usage is to show a splash 
  screen before the main widget is displayed on the screen. 
  For example,
  \code
  int main(int argc, char *argv[])
  {
    QApplication app(argc, argv);
    QPixmap pixmap(":/splash.png");
    QtxSplash* splash = QtxsSplash::splash(pixmap);
    splash->show();
    app.processEvents();
    ... // do application loading and initialization
    MainWindow window;
    window.show();
    splash->finish(&window);
    return app.exec();
  }
  \endcode
  
  The user can hide the splash screen by clicking on it with the mouse. Since the splash screen is 
  typically displayed before the event loop has started running, it is necessary to periodically call 
  QApplication::processEvents() to receive the mouse clicks.
  This feature can be switched off by using method setHideOnClick() with \c false parameter.

  It is sometimes useful to update the splash screen with messages and/or progress information, 
  for example, announcing connections established or modules loaded as the application starts up.
  QtxSplash class provides the functionality to show status messages and(or) progress bar.

  \code
  QPixmap pixmap(":/splash.png");
  QtxSplash* splash = QtxSplash::splash(pixmap);
  splash->setProgress(0, 5);
  splash->show();
  app.processEvents();
  // doing first step
  splash->message("Step 1");
  splash->ress(1);
  qApp->processEvents();
  // ... perform some actions
  // doing second step
  splash->message("Step 2");
  splash->setProgress(2);
  qApp->processEvents();
  // ... perform some actions
  ... et cetera
  \endcode

  There is a static function QtxSplash::setStatus() which allows to put next status message
  and progress with one call. It can substitue two calls: message() and setProgress().

  QtxSplash class provides alos a lot of functions to set-up its behavior. Set progress
  bar width with setProgressWidth() method, its position and direction with setProgressFlags().
  It can be single-colored or gradient-colored. Use setProgressColors() method for this. 
  You can even set your own gradient scale with QLinearGradient and use it for the progress
  bar coloring: setProgressGradient().

  To change the progress bar and status message transparency, use setOpacity() function.
  The methods setTextAlignment(), setTextColor() and setTextColors() can be used to change
  the attributes of the status message.
*/

//! The only one instance of splash screen
QtxSplash* QtxSplash::mySplash = 0;

/*!
  \brief Constructor.
  \brief Construct a splash screen that will display the \a pixmap.
  \param pixmap splash screen pixmap
*/
QtxSplash::QtxSplash( const QPixmap& pixmap )
: QWidget( 0, Qt::SplashScreen | Qt::WindowStaysOnTopHint ),
  myAlignment( Qt::AlignBottom | Qt::AlignRight ),
  myColor( Qt::white ),
  myHideOnClick( false ),
  myProgress( 0 ),
  myTotal( 0 ),
  myStartColor( Qt::red ),
  myGradientType( Vertical ),
  myProgressWidth( 10 ),
  myProgressFlags( BottomSide | LeftToRight ),
  myMargin( 5 ),
  myOpacity( 1.0 ),
  myError( 0 ),
  myGradientUsed( false )
{
  setAttribute( Qt::WA_DeleteOnClose, true );
  setPixmap( pixmap );
}

/*!
  \brief Destructor.
*/
QtxSplash::~QtxSplash()
{
  mySplash = 0;
}

/*!
  \brief Get the only instance of the splash screen widget.

  If the splash screen widget does not exist yet, it is created with specified
  pixmap. Otherwise, pixmap \a px is set to existing widget.

  \param px splash screen pixmap
  \return splash screen widget
*/
QtxSplash* QtxSplash::splash( const QPixmap& px )
{
  if ( !mySplash )
    mySplash = new QtxSplash( px );
  else if ( !px.isNull() )
    mySplash->setPixmap( px );
  return mySplash;
}

/*!
  \brief Send the status message and (optionally) current progress 
  to the splash screen.

  This function can be used, for example, from an external thread
  which checks the application loading progress.

  \param msg progress status message
  \param progress current progress
  \sa message(), setProgress()
*/
void QtxSplash::setStatus( const QString& msg, const int progress )
{
  if ( mySplash ) {
    QApplication::postEvent( mySplash, new ProgressEvent( msg, progress ) );
    QApplication::instance()->processEvents();
  }
}

/*!
  \brief Set error status and show error message box to the user.
  \param error error message
  \param title message box title
  \param code error code
*/
void QtxSplash::error( const QString& error, const QString& title, const int code )
{
  if ( mySplash ) {
    mySplash->setError( code );
    QMessageBox::critical( mySplash, 
			   title.isEmpty() ? tr( "Error" ) : title,
			   error,
			   tr( "&OK" ) );
  }
  else {
    printf( "QtxSplash::error: %s\n",error.toLatin1().constData() );
  }
}

/*!
  \brief Set the pixmap that will be used as the splash screen's image.
  \param pixmap spash screen image pixmap
  \sa pixmap()
*/
void QtxSplash::setPixmap( const QPixmap& pixmap )
{
  if ( pixmap.hasAlpha() ) {
    QPixmap opaque( pixmap.size() );
    QPainter p( &opaque );
    p.fillRect( 0, 0, pixmap.width(), pixmap.height(), palette().background() );
    p.drawPixmap( 0, 0, pixmap );
    p.end();
    myPixmap = opaque;
  } 
  else {
    myPixmap = pixmap;
  }
  QRect r( 0, 0, myPixmap.size().width(), myPixmap.size().height() );
  resize( myPixmap.size() );
  move( QApplication::desktop()->screenGeometry().center() - r.center() );
  if ( !isVisible() )
    drawContents();
  else
    repaint();
}

/*!
  \brief Get the pixmap that is used as the splash screen's image.
  \return spash screen image pixmap
  \sa setPixmap()
*/
QPixmap QtxSplash::pixmap() const
{
  return myPixmap;
}

/*!
  \brief Set/clear the 'hide on mouse click' flag.

  When this flag is set, user can hide the splash screen window
  by clicking on it with mouse.
  But for this to work it is necessary to call periodically
  QApplication::processEvents() in order to allow event loop to process
  events because usually main application loop is not yet started
  at that moment.

  By default this flag is set to \c false.

  \param on new flag state
  \sa hideOnClick()
*/
void QtxSplash::setHideOnClick( const bool on )
{
  myHideOnClick = on;
}

/*!
  \brief Get the 'hide on mouse click' flag.
  \return 'hide on mouse click' flag
  \sa setHideOnClick()
*/
bool QtxSplash::hideOnClick() const
{
  return myHideOnClick;
}

/*!
  \brief Set total progress steps to \a total.
  \param total total number of progress steps
  \sa totalSteps(), setProgress(), progress()
*/
void QtxSplash::setTotalSteps( const int total )
{
  myTotal = total;
  repaint();
}

/*!
  \brief Get total progress steps number.
  \return total number of progress steps
  \sa setTotalSteps(), setProgress(), progress()
*/
int QtxSplash::totalSteps() const
{
  return myTotal;
}
    
/*!
  \brief Set current progress.
  \param progress current progress
  \sa progress(), setTotalSteps(), setTotalSteps(), 
*/
void QtxSplash::setProgress( const int progress )
{
  myProgress = progress > 0 ? progress : 0;
  repaint();
}

/*!
  \brief Get current progress.
  \return current progress
  \sa setProgress(), setTotalSteps(), setTotalSteps(), 
*/
int QtxSplash::progress() const
{
  return myProgress;
}

/*!
  \brief Set current progress to \a progress and total number of 
  progress steps to \a total.
  \param progress current progress
  \param total total number of progress steps
*/
void QtxSplash::setProgress( const int progress, const int total )
{
  myTotal    = total;
  myProgress = progress > 0 ? progress : 0;
  repaint();
}

/*!
  \brief Set margin (a border width).
  \param margin new margin width
  \sa margin()
*/
void QtxSplash::setMargin( const int margin )
{
  myMargin = margin > 0 ? margin : 0;
  repaint();
}

/*!
  \brief Get margin (a border width).
  \return current margin width
  \sa setMargin()
*/
int QtxSplash::margin() const
{
  return myMargin;
}

/*!
  \brief Set progress bar width.
  \param width new progress bar width
  \sa progressWidth()
*/
void QtxSplash::setProgressWidth( const int width )
{
  myProgressWidth = width > 0 ? width : 0;
  repaint();
}

/*!
  \brief Get progress bar width.
  \return current progress bar width
  \sa setProgressWidth()
*/
int QtxSplash::progressWidth() const
{
  return myProgressWidth;
}

/*!
  \brief Set progress bar position and direction.

  By default, progress bar is displayed at the bottom side and
  shows progress from left to right.

  \param flags ORed progress bar flags (QtxSplash::ProgressBarFlags)
  \sa progressFlags()
*/
void QtxSplash::setProgressFlags( const int flags )
{
  myProgressFlags = flags;
  if ( !( myProgressFlags & ( LeftSide | RightSide | TopSide | BottomSide ) ) )
    myProgressFlags |= BottomSide;
  if ( !( myProgressFlags & ( LeftToRight | RightToLeft ) ) )
    myProgressFlags |= LeftToRight ;
  repaint();
}

/*!
  \brief Get progress bar flags: position and direction.
  \return ORed progress bar flags (QtxSplash::ProgressBarFlags)
  \sa setProgressFlags()
*/
int QtxSplash::progressFlags() const
{
  return myProgressFlags;
}

/*!
  \brief Set progress bar colors.

  If the colors differ the gradient color bar is drawn.
  
  If the \a endColor is not valid, \a startColor is used instead
  (no gradient coloring).
  
  The parameter \a gradientType defines the type of gradient 
  to be drawn - horizontal or vertical. Default is vertical.

  Note, that methods setProgressGradient() and setProgressColors() are
  alternative. Only the latest used is taken into account.

  \param startColor start gradient color (or mono-color)
  \param endColor end gradient color
  \param gradientType gradient type (QtxSplash::GradientType)
  \sa progressColors(), setProgressGradient()
*/
void QtxSplash::setProgressColors( const QColor&      startColor, 
				   const QColor&      endColor,
                                   const GradientType gradientType )
{
  if ( startColor.isValid() )
    myStartColor = startColor;
  myEndColor     = endColor;
  myGradientType = gradientType;
  myGradientUsed = false;
  repaint();
}

/*!
  \brief Get progress colors and gradient type.
  \param startColor start gradient color (or mono-color)
  \param endColor end gradient color
  \return gradient type (QtxSplash::GradientType)
  \sa setProgressColors()
*/
QtxSplash::GradientType QtxSplash::progressColors( QColor& startColor, 
						   QColor& endColor ) const
{
  startColor = myStartColor;
  endColor   = myEndColor;
  return myGradientType;
}

/*!
  \brief Set custom progress bar colors.

  The gradient start and final stops are scaled to the actual progress
  bar size. For example:
  \code
  QLinearGradient lg(0.5, 0, 1, 1);
  lg.setColorAt(0.2, Qt::blue);
  lg.setColorAt(0.6, Qt::red);
  lg.setSpread(QGradient::RepeatSpread);
  splash->setProgressGradient(lg);
  \endcode
  The above code creates linear gradient, which sets start stop to the
  center of the progress bar; the final stop is always in the end of
  the progress bar. The color scale (blue to red) is changed by the 
  progress bar diagonal.

  Note, that methods setProgressGradient() and setProgressColors() are
  alternative. Only the latest used is taken into account.

  \param gradient color gradient to be used for progress bar coloring
  \sa progressGradient(), setProgressColors()
*/
void QtxSplash::setProgressGradient( const QLinearGradient& gradient )
{
  myGradient = gradient;
  myGradientUsed = true;
  repaint();
}

/*!
  \brief Get custom progress bar colors.
  \return color gradient used for progress bar coloring
  \sa setProgressGradient()
*/
QLinearGradient QtxSplash::progressGradient() const
{
  return myGradient;
}

/*!
  \brief Set progress bar and status text message opacity.

  The value should be in the range 0.0 to 1.0, where 0.0 is fully 
  transparent and 1.0 is fully opaque.

  \param opacity new opacity value
  \sa opacity()
*/
void QtxSplash::setOpacity( const double opacity )
{
  myOpacity = opacity < 0.0 ? 0.0 : ( opacity > 1.0 ? 1.0 : opacity );
  repaint();
}

/*!
  \brief Get progress bar and status text message opacity.
  \return current opacity value
  \sa setOpacity()
*/
double QtxSplash::opacity() const
{
  return myOpacity;
}

/*!
  \brief Set message text alignment flags.

  Default flags are Qt::AlignBottom | Qt::AlignRight.

  \param alignment alignment flags (Qt::Alignment)
  \sa textAlignment()
*/
void QtxSplash::setTextAlignment( const int alignment )
{
  myAlignment = alignment;
  repaint();
}

/*!
  \brief Get message text alignment flags.
  \return alignment flags (Qt::Alignment)
  \sa setTextAlignment()
*/
int QtxSplash::textAlignment() const
{
  return myAlignment;
}

/*!
  \brief Set message text color.

  Default message color is white.
  
  \param color message text color
  \sa setTextColors()
*/
void QtxSplash::setTextColor( const QColor& color )
{
  if ( myColor.isValid() )
    myColor = color;
  myShadowColor = QColor();
  repaint();
}

/*!
  \brief Get message text color.
  \return color message text color
  \sa setTextColor()
*/
QColor QtxSplash::textColor() const
{
  return myColor;
}

/*!
  \brief Set message text color and text shadow color.
  \param color message text color
  \param shadow message text shadow color
  \sa textColors(), textColor(), setTextColor()
*/
void QtxSplash::setTextColors( const QColor& color, const QColor& shadow )
{
  if ( myColor.isValid() )
    myColor = color;
  myShadowColor = shadow;
  repaint();
}

/*!
  \brief Get message text color and text shadow color.
  \param color message text color
  \param shadow message text shadow color
  \sa setTextColors(), textColor(), setTextColor()
*/
void QtxSplash::textColors( QColor& color, QColor& shadow ) const
{
  color  = myColor;
  shadow = myShadowColor;
}

/*!
  \brief Get current status message.
  \return status message
*/
QString QtxSplash::message() const
{
  return myMessage;
}

/*!
  \brief Get error code.

  This function returns error code, set previoiusly with 
  error(const QString&, const QString&, const int) method.
  If no error code has been set, 0 is returned.

  \return last error code
*/
int QtxSplash::error() const
{
  return myError;
}

/*!
  \brief Wait until widget \a mainWin is displayed.

  Makes the splash screen wait until the widget \a mainWin is displayed
  and then hide and close splash window.

  \param mainWin application main window 
*/
void QtxSplash::finish( QWidget* mainWin )
{
  if ( mainWin ) {
#if defined(Q_WS_X11)
    extern void qt_x11_wait_for_window_manager(QWidget *mainWin);
    qt_x11_wait_for_window_manager(mainWin);
#endif
  }
  close();
}

/*!
  \brief Repaint the splash screen.
*/
void QtxSplash::repaint()
{
  drawContents();
  QWidget::repaint();
  QApplication::flush();
}

/*!
  \brief Set status message for the splash screen and define its color 
  and aligment flags.
  \param msg status message
  \param alignment message text alignment flags (Qt::Alignment)
  \param color message text color
*/
void QtxSplash::message( const QString& msg, 
			 int            alignment,
			 const QColor&  color )
{
  myMessage   = msg;
  myAlignment = alignment;
  if ( color.isValid() )
    myColor = color;
  repaint();
}

/*!
  \overload
  \brief Set status message for the splash screen.
  \param msg status message
*/
void QtxSplash::message( const QString& msg )
{
  myMessage = msg;
  repaint();
}

/*!
  \brief Remove the message being displayed on the splash screen.
  \sa message()
*/
void QtxSplash::clear()
{
  myMessage = QString::null;
  repaint();
}

/*!
  \brief Draw the contents of the splash screen.
  \param painter painter
*/
void QtxSplash::drawContents( QPainter* p )
{
  // draw progress bar
  if ( myTotal > 0 ) {
    p->save();
    drawProgressBar( p );
    p->restore();
  }

  // draw status message
  if ( !myMessage.isEmpty() ) {
    p->save();
    drawMessage( p );
    p->restore();
  }
}

/*!
  \brief Process mouse button pressing event.

  Hides splash screen if the 'hide on mouse click' flag is set.

  \param me mouse event (not used)
  \sa hideOnClick(), setHideOnClick()
*/
void QtxSplash::mousePressEvent( QMouseEvent* /*me*/ )
{
  if ( myHideOnClick )
    hide();
}

/*!
  \brief Customize paint event.

  This function is implemented to work-around the Qt bug
  on some Linux distribututions when the drawing on the 
  splash screen widget is not allowed.

  \param pe paint event (not used)
*/
void QtxSplash::paintEvent( QPaintEvent* /*pe*/ )
{
  QPainter p( this );
  QPixmap pix = palette().brush( backgroundRole() ).texture();
  p.drawPixmap( 0, 0, pix );
}

/*!
  \brief Process custom event sent by setStatus() method.
  \param ce custom event
  \sa setStatus().
*/
void QtxSplash::customEvent( QEvent* ce )
{
  if ( ce->type() == ProgressEvent::id() ) {
    ProgressEvent* pe = (ProgressEvent*)ce;
    pe->message().isEmpty() ? clear() : message( pe->message() );
    setProgress( pe->progress() );
    QApplication::instance()->processEvents();
  }
}

/*!
  \brief Draw progress bar.
  \param p painter
*/
void QtxSplash::drawProgressBar( QPainter* p )
{
  // get rect, margin, progress bar width
  QRect r = rect();
  int m   = margin();
  int pw  = progressWidth();

  // calculate drawing rect
  // ... first set default position (if none or wrong position is set)
  if ( myProgressFlags & BottomSide )
    r = QRect( r.x() + m, r.height() - (m + pw), r.width() - 2 * m, pw );
  else if ( myProgressFlags & TopSide )
    r = QRect( r.x() + m, r.y() + m, r.width() - 2 * m, pw );
  else if ( myProgressFlags & LeftSide )
    r = QRect( r.x() + m, r.y() + m, pw, r.height() - 2 * m );
  else if ( myProgressFlags & RightSide )
    r = QRect( r.width() - (m + pw), r.y() + m, pw, r.height() - 2 * m );

  QRect cr = r;
  if ( myProgressFlags & TopSide || myProgressFlags & BottomSide ) {
    cr.setWidth( (int)( r.width() * ( myProgress > 0 ? myProgress : 0 ) / myTotal ) );
    if ( myProgressFlags & RightToLeft )
      cr.translate( r.width() - cr.width(), 0 );
  }
  else if ( myProgressFlags & LeftSide || myProgressFlags & RightSide ) {
    cr.setHeight( (int)( r.height() * ( myProgress > 0 ? myProgress : 0 ) / myTotal ) );
    if ( myProgressFlags & RightToLeft)
      cr.translate( 0, r.height() - cr.height() );
  }
  int x1, x2, y1, y2;
  if ( myGradientType == Horizontal ) {
    x1 = r.left(); x2 = r.right(); y1 = y2 = 0;
  }
  else {
    x1 = x2 = 0; y1 = r.top(); y2 = r.bottom();
  }
  QLinearGradient lg;
  if ( myGradientUsed ) {
    QPointF start = myGradient.start();
    QPointF final = myGradient.finalStop();
    qreal xd = final.x() - start.x();
    qreal yd = final.y() - start.y();
    lg.setStart( xd != 0 ? r.left() + r.width() * start.x() / xd : 0, 
                 yd != 0 ? r.top() + r.height() * start.y() / yd : 0 );
    lg.setFinalStop( xd != 0 ? r.right() : 0, yd != 0 ? r.bottom() : 0 );
    lg.setStops( myGradient.stops() );
    lg.setSpread( myGradient.spread() );
  }
  else {
    lg.setStart( x1, y1 );
    lg.setFinalStop( x2, y2 );
    lg.setColorAt( 0, myStartColor );
    lg.setColorAt( 1, myEndColor.isValid() ? myEndColor : myStartColor );
  }
  p->setOpacity( myOpacity );
  p->setClipRect( cr );
  p->fillRect( r, lg ); 
  p->setClipping( false );

  // draw progress bar outline rectangle
  p->setPen( palette().color( QPalette::Dark ) );
  p->drawLine( r.left(), r.top(), r.right(), r.top() );
  p->drawLine( r.left(), r.top(), r.left(), r.bottom() );
  p->setPen( palette().color( QPalette::Light ) );
  p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
  p->drawLine( r.right(), r.top(), r.right(), r.bottom() );
}

/*!
  \brief Draw status message.
  \param p painter
*/
void QtxSplash::drawMessage( QPainter* p )
{
  // get rect, margin, progress bar width
  QRect r = rect();
  int m   = margin();
  int pw  = progressWidth();

  // calculate drawing rect
  QFontMetrics f( font() );
  int spacing = f.lineSpacing();
  // ... base rect
  QRect r1( r.x() + m, r.y() + m, r.width() - 2 * m, r.height() - 2 * m );
  r1.setY( r1.y() - f.leading() );
  // ... take into account progress bar
  if ( 1 ) {              // if ( myTotal > 0 ) : vsr changed: otherwise text is jumping
    if ( myProgressFlags & BottomSide )
      r1.setHeight( r1.height() - pw );
    else if ( myProgressFlags & TopSide )
      r1.setY( r1.y() + pw );
    else if ( myProgressFlags & LeftSide )
      r1.setX( r1.x() + pw );
    else if ( myProgressFlags & RightSide )
      r1.setWidth( r1.width() - pw );
  }
  
  // ... take into account trailing '\n' symbols
  int shift = 0;
  int i = myMessage.length() - 1;
  while( i >= 0 && myMessage[ i-- ] == '\n' )
    shift += spacing;
  r1.setHeight( r1.height() - shift );

  p->setOpacity( myOpacity );

  // draw shadow status text
  if ( myShadowColor.isValid() ) {
    QRect r2 = r1;
    if ( myAlignment & Qt::AlignLeft   ) r2.setLeft  ( r2.left()   + 1 );
    if ( myAlignment & Qt::AlignTop    ) r2.setTop   ( r2.top()    + 1 );
    if ( myAlignment & Qt::AlignRight  ) r2.setRight ( r2.right()  + 1 );
    if ( myAlignment & Qt::AlignBottom ) r2.setBottom( r2.bottom() + 1 );
    p->setPen( myShadowColor );
    p->drawText( r2, myAlignment, myMessage );
  }

  // draw foreground status text
  p->setPen( myColor );
  p->drawText( r1, myAlignment, myMessage );
}

/*!
  \brief Draw the splash screen window contents.
*/
void QtxSplash::drawContents()
{
  QPixmap textPix = myPixmap;
  QPainter painter( &textPix );
  painter.initFrom( this );
  drawContents( &painter );
  QPalette pal = palette();
  pal.setBrush( backgroundRole(), QBrush( textPix ) );
  setPalette( pal );
}

/*!
  \brief Sets error code.
  \param code error code
*/
void QtxSplash::setError( const int code )
{
  myError = code;
}

